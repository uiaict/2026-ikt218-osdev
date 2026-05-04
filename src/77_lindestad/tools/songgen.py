#!/usr/bin/env python3
"""Generate kernel song data and optional preview audio from a text melody."""

from __future__ import annotations

import argparse
import shutil
import struct
import subprocess
import sys
import wave
from dataclasses import dataclass
from pathlib import Path


PIT_BASE_FREQUENCY = 1_193_180
SAMPLE_RATE = 44_100

PYTHAGOREAN_RATIOS = {
    "C": 1.0,
    "Cs": 2187 / 2048,
    "D": 9 / 8,
    "Ds": 19683 / 16384,
    "E": 81 / 64,
    "F": 4 / 3,
    "Fs": 729 / 512,
    "G": 3 / 2,
    "Gs": 6561 / 4096,
    "A": 27 / 16,
    "As": 59049 / 32768,
    "B": 243 / 128,
}

FLAT_TO_SHARP = {
    "Db": "Cs",
    "Eb": "Ds",
    "Gb": "Fs",
    "Ab": "Gs",
    "Bb": "As",
}

DURATION_BEATS = {
    "whole": 4.0,
    "half": 2.0,
    "quarter": 1.0,
    "eighth": 0.5,
    "8th": 0.5,
    "eigth": 0.5,
    "sixteenth": 0.25,
    "16th": 0.25,
    "32nd": 0.125,
}


@dataclass(frozen=True)
class Note:
    pitch: str
    beats: float
    measure: str


@dataclass(frozen=True)
class Song:
    title: str
    identifier: str
    tempo: int
    notes: list[Note]


def normalize_pitch(pitch: str) -> str:
    if pitch == "R":
        return pitch

    note = pitch[:-1].replace("#", "s")
    octave = pitch[-1]
    note = FLAT_TO_SHARP.get(note, note)
    return f"{note}{octave}"


def frequency_for_pitch(pitch: str) -> int:
    pitch = normalize_pitch(pitch)
    if pitch == "R":
        return 0

    name = pitch[:-1]
    octave = int(pitch[-1])
    if name not in PYTHAGOREAN_RATIOS:
        raise ValueError(f"unknown pitch: {pitch}")
    c4 = 440.0 / PYTHAGOREAN_RATIOS["A"]
    frequency = c4 * PYTHAGOREAN_RATIOS[name] * (2 ** (octave - 4))
    return int(round(frequency))


def parse_beats(value: str) -> float:
    if "/" in value:
        numerator, denominator = value.split("/", 1)
        return float(numerator) / float(denominator)
    return float(value)


def parse_duration(parts: list[str]) -> float:
    beats = 0.0
    last_value = 0.0

    for part in parts:
        token = part.lower()
        if token == "plus":
            continue
        if token == "dotted":
            if last_value <= 0:
                raise ValueError("'dotted' must follow a duration")
            beats += last_value / 2
            continue

        if token in DURATION_BEATS:
            value = DURATION_BEATS[token]
        else:
            value = parse_beats(token)

        beats += value
        last_value = value

    if beats <= 0:
        raise ValueError("duration must be greater than zero")
    return beats


def parse_song(path: Path) -> Song:
    title = "Generated song"
    identifier = "generated_song"
    tempo = 84
    notes: list[Note] = []
    current_measure = "?"

    for line_number, raw_line in enumerate(path.read_text().splitlines(), start=1):
        line = raw_line.split("#", 1)[0].strip()
        if not line:
            continue

        parts = line.split()
        directive = parts[0].lower()

        if directive == "title":
            title = " ".join(parts[1:])
            continue
        if directive == "identifier":
            identifier = parts[1]
            continue
        if directive == "tempo":
            tempo = int(parts[1])
            continue
        if directive == "measure":
            current_measure = parts[1]
            continue

        if parts[0].lower() == "pause":
            parts[0] = "R"

        if len(parts) < 2:
            raise ValueError(f"{path}:{line_number}: expected '<pitch> <duration>'")

        pitch = normalize_pitch(parts[0])
        try:
            beats = parse_duration(parts[1:])
            frequency_for_pitch(pitch)
        except ValueError as error:
            raise ValueError(f"{path}:{line_number}: {error}") from error
        notes.append(Note(pitch=pitch, beats=beats, measure=current_measure))

    if not notes:
        raise ValueError(f"{path}: no notes found")

    return Song(title=title, identifier=identifier, tempo=tempo, notes=notes)


def duration_ms(beats: float, tempo: int) -> int:
    return int(round(beats * 60_000 / tempo))


def pit_divisor(frequency: int) -> int:
    if frequency == 0:
        return 0
    return max(1, int(round(PIT_BASE_FREQUENCY / frequency)))


def emit_c(song: Song, header_path: Path, source_path: Path) -> None:
    array_name = f"{song.identifier}_notes"
    header_path.write_text(
        "\n".join(
            [
                "#pragma once",
                "",
                "#include <song.h>",
                "",
                f"extern const Note {array_name}[];",
                f"extern const Song {song.identifier};",
                "",
            ]
        )
    )

    rows = []
    for note in song.notes:
        frequency = frequency_for_pitch(note.pitch)
        ms = duration_ms(note.beats, song.tempo)
        divisor = pit_divisor(frequency)
        rows.append(
            f"    {{{frequency}, {ms}}}, /* m. {note.measure}: {note.pitch}, "
            f"{note.beats:g} beat(s), divisor {divisor} */"
        )

    source_path.write_text(
        "\n".join(
            [
                f'#include "{header_path.name}"',
                "",
                f"const Note {array_name}[] = {{",
                *rows,
                "};",
                "",
                f"const Song {song.identifier} = {{",
                f"    (Note*){array_name},",
                f"    {len(song.notes)},",
                f'    "{song.title}",',
                "};",
                "",
            ]
        )
    )


def render_wav(song: Song, wav_path: Path) -> None:
    wav_path.parent.mkdir(parents=True, exist_ok=True)
    amplitude = 11_000
    articulation_gap_ms = 28

    with wave.open(str(wav_path), "wb") as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(SAMPLE_RATE)

        for note in song.notes:
            frequency = frequency_for_pitch(note.pitch)
            total_ms = duration_ms(note.beats, song.tempo)
            gap_ms = articulation_gap_ms if frequency and total_ms > 90 else 0
            tone_ms = max(0, total_ms - gap_ms)

            for sample_index in range(int(SAMPLE_RATE * tone_ms / 1000)):
                if frequency == 0:
                    sample = 0
                else:
                    period = SAMPLE_RATE / frequency
                    sample = amplitude if (sample_index % period) < (period / 2) else -amplitude
                wav.writeframes(struct.pack("<h", int(sample)))

            for _ in range(int(SAMPLE_RATE * gap_ms / 1000)):
                wav.writeframes(struct.pack("<h", 0))


def play_wav(wav_path: Path) -> int:
    last_status = 1
    for command in ("paplay", "aplay", "pw-play", "ffplay"):
        executable = shutil.which(command)
        if executable is None:
            continue
        if command == "ffplay":
            last_status = subprocess.call([executable, "-nodisp", "-autoexit", str(wav_path)])
        else:
            last_status = subprocess.call([executable, str(wav_path)])
        if last_status == 0:
            return 0

    print("could not play audio; wrote wav only", file=sys.stderr)
    return last_status


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("song", type=Path)
    parser.add_argument("--header", type=Path)
    parser.add_argument("--source", type=Path)
    parser.add_argument("--wav", type=Path)
    parser.add_argument("--play", action="store_true")
    args = parser.parse_args()

    song = parse_song(args.song)

    if args.header and args.source:
        emit_c(song, args.header, args.source)
        print(f"generated {args.header} and {args.source}")

    if args.wav:
        render_wav(song, args.wav)
        print(f"wrote {args.wav}")

    if args.play:
        if args.wav is None:
            print("--play requires --wav", file=sys.stderr)
            return 2
        return play_wav(args.wav)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
