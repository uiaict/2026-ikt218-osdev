#include "brahms_op38_opening.h"

const Note brahms_op38_opening_notes[] = {
    {82, 1395}, /* m. 1: E2, 2 beat(s), divisor 14551 */
    {98, 1047}, /* m. 1: G2, 1.5 beat(s), divisor 12175 */
    {124, 174}, /* m. 1: B2, 0.25 beat(s), divisor 9622 */
    {130, 1395}, /* m. 2: C3, 2 beat(s), divisor 9178 */
    {124, 1395}, /* m. 2: B2, 2 beat(s), divisor 9622 */
    {110, 349}, /* m. 3: A2, 0.5 beat(s), divisor 10847 */
    {124, 174}, /* m. 3: B2, 0.25 beat(s), divisor 9622 */
    {110, 174}, /* m. 3: A2, 0.25 beat(s), divisor 10847 */
    {98, 174}, /* m. 3: G2, 0.25 beat(s), divisor 12175 */
    {110, 349}, /* m. 3: A2, 0.5 beat(s), divisor 10847 */
    {124, 1047}, /* m. 3: B2, 1.5 beat(s), divisor 9622 */
    {98, 349}, /* m. 3: G2, 0.5 beat(s), divisor 12175 */
    {82, 1395}, /* m. 4: E2, 2 beat(s), divisor 14551 */
    {93, 1744}, /* m. 4: Fs2, 2.5 beat(s), divisor 12830 */
    {78, 349}, /* m. 5: Ds2, 0.5 beat(s), divisor 15297 */
    {93, 349}, /* m. 5: Fs2, 0.5 beat(s), divisor 12830 */
    {124, 349}, /* m. 5: B2, 0.5 beat(s), divisor 9622 */
    {157, 1047}, /* m. 5: Ds3, 1.5 beat(s), divisor 7600 */
    {186, 349}, /* m. 5: Fs3, 0.5 beat(s), divisor 6415 */
    {186, 1395}, /* m. 6: Fs3, 2 beat(s), divisor 6415 */
    {93, 1395}, /* m. 6: Fs2, 2 beat(s), divisor 12830 */
    {196, 1395}, /* m. 7: G3, 2 beat(s), divisor 6088 */
    {93, 2093}, /* m. 7: Fs2, 3 beat(s), divisor 12830 */
    {104, 174}, /* m. 7: Gs2, 0.25 beat(s), divisor 11473 */
    {117, 174}, /* m. 7: As2, 0.25 beat(s), divisor 10198 */
    {124, 1395}, /* m. 8: B2, 2 beat(s), divisor 9622 */
    {0, 1395}, /* m. 8: R, 2 beat(s), divisor 0 */
};

const Song brahms_op38_opening = {
    (Note*)brahms_op38_opening_notes,
    27,
    "Brahms Op. 38 opening theme excerpt",
};
