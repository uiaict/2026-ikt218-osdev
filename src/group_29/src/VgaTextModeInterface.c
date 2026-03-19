#pragma once
//VGA resolution 80 x 25
struct VgaTextModeCursor{
    uint8_t row;
    uint8_t col;
    uint16_t* memory_position;
    uint16_t* memory_start;
    uint16_t* memory_end;
    void (*CalculateRowColFromMemoryPosition)(struct VgaTextModeCursor*);
};

/** Use NewVgaTextModeInterface() to create an instance of this struct */
struct VgaTextModeInterface{
    struct VgaTextModeCursor cursor;
    /** \brief Print a 0 terminated char array to the screen
     * \param attribute Attribute to set for the text. Applies to the entire string. See enum standard_vga_text_color
     * \param input An pointer to an array of ASCII characters, terminated by 0.
    */
    void (*Print)(struct VgaTextModeInterface*, char* input, uint8_t attribute);
};

/** \brief See VgaTextModeInterface.Print() */
void VgaTextModeInterfacePrint(struct VgaTextModeInterface* a, char* input, uint8_t attribute){
    while ( *input != 0 && (a->cursor.memory_position < a->cursor.memory_end)){
        *(a->cursor.memory_position) = (uint16_t)(attribute<<8|*(input));   
        a->cursor.memory_position++;
        input++;
    }
    a->cursor.CalculateRowColFromMemoryPosition(&(a->cursor));
}

/** \brief See VgaTextModeCursor.CalculateRowColFromMemoryPosition()
 * \todo Check that this works correctly
 */
void VgaTextModeCursorCalculateRowColFromMemoryPosition(struct VgaTextModeCursor* c){
    uint32_t z = (uint32_t)c->memory_position - (uint32_t)c->memory_start;
    c->row = z / 80;
    c->col = z % 80;
}

/** \brief Initialises a VgaTextModeInterface and returns it. */
struct VgaTextModeInterface NewVgaTextModeInterface(){
    struct VgaTextModeInterface i;
    i.Print = VgaTextModeInterfacePrint;
    i.cursor.row = 0;
    i.cursor.col = 0;
    i.cursor.memory_position = (uint16_t*)0xb8000;
    i.cursor.memory_start = (uint16_t*)0xb8000;
    i.cursor.memory_end = (uint16_t*)((25 * 80 * 2) + 0xb8000);
    i.cursor.CalculateRowColFromMemoryPosition = VgaTextModeCursorCalculateRowColFromMemoryPosition;
    return i;
}