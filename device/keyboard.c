#include "stdint.h"
#include "print.h"
#include "io.h"
#include "keyboard.h"
#include "string.h"

// 以下定义全部来自操作系统真相还原.
#define esc		                    '\033'	 
#define backspace	                '\b'
#define tab		                    '\t'
#define enter                       '\r'
#define delete		                '\177'

/* 以上不可见字符一律定义为0 */
#define char_invisible	            0
#define ctrl_l_char	                char_invisible
#define ctrl_r_char	                char_invisible
#define shift_l_char	            char_invisible
#define shift_r_char	            char_invisible
#define alt_l_char	                char_invisible
#define alt_r_char	                char_invisible
#define caps_lock_char	            char_invisible

/* 定义控制字符的通码和断码 */
#define shift_l_make	            0x2a
#define shift_r_make 	            0x36 
#define alt_l_make   	            0x38
#define alt_r_make   	            0xe038
#define alt_r_break   	            0xe0b8
#define ctrl_l_make  	            0x1d
#define ctrl_r_make  	            0xe01d
#define ctrl_r_break 	            0xe09d
#define caps_lock_make 	            0x3a


/* 定义以下变量记录相应键是否按下的状态,
 * ext_scancode用于记录makecode是否以0xe0开头 */
static uint8_t ctrl_status = 0, shift_status = 0, alt_status = 0, 
               caps_lock_status = 0, ext_scancode = 0;

/* 以通码make_code为索引的二维数组 */
static uint32_t keymap[][2] = {
/* 扫描码   未与shift组合  与shift组合*/
/* ---------------------------------- */
/* 0x00 */	{0,	0},		
/* 0x01 */	{esc,	esc},		
/* 0x02 */	{'1',	'!'},		
/* 0x03 */	{'2',	'@'},		
/* 0x04 */	{'3',	'#'},		
/* 0x05 */	{'4',	'$'},		
/* 0x06 */	{'5',	'%'},		
/* 0x07 */	{'6',	'^'},		
/* 0x08 */	{'7',	'&'},		
/* 0x09 */	{'8',	'*'},		
/* 0x0A */	{'9',	'('},		
/* 0x0B */	{'0',	')'},		
/* 0x0C */	{'-',	'_'},		
/* 0x0D */	{'=',	'+'},		
/* 0x0E */	{backspace, backspace},	
/* 0x0F */	{tab,	tab},		
/* 0x10 */	{'q',	'Q'},		
/* 0x11 */	{'w',	'W'},		
/* 0x12 */	{'e',	'E'},		
/* 0x13 */	{'r',	'R'},		
/* 0x14 */	{'t',	'T'},		
/* 0x15 */	{'y',	'Y'},		
/* 0x16 */	{'u',	'U'},		
/* 0x17 */	{'i',	'I'},		
/* 0x18 */	{'o',	'O'},		
/* 0x19 */	{'p',	'P'},		
/* 0x1A */	{'[',	'{'},		
/* 0x1B */	{']',	'}'},		
/* 0x1C */	{enter,  enter},
/* 0x1D */	{ctrl_l_char, ctrl_l_char},
/* 0x1E */	{'a',	'A'},		
/* 0x1F */	{'s',	'S'},		
/* 0x20 */	{'d',	'D'},		
/* 0x21 */	{'f',	'F'},		
/* 0x22 */	{'g',	'G'},		
/* 0x23 */	{'h',	'H'},		
/* 0x24 */	{'j',	'J'},		
/* 0x25 */	{'k',	'K'},		
/* 0x26 */	{'l',	'L'},		
/* 0x27 */	{';',	':'},		
/* 0x28 */	{'\'',	'"'},		
/* 0x29 */	{'`',	'~'},		
/* 0x2A */	{shift_l_char, shift_l_char},	
/* 0x2B */	{'\\',	'|'},		
/* 0x2C */	{'z',	'Z'},		
/* 0x2D */	{'x',	'X'},		
/* 0x2E */	{'c',	'C'},		
/* 0x2F */	{'v',	'V'},		
/* 0x30 */	{'b',	'B'},		
/* 0x31 */	{'n',	'N'},		
/* 0x32 */	{'m',	'M'},		
/* 0x33 */	{',',	'<'},		
/* 0x34 */	{'.',	'>'},		
/* 0x35 */	{'/',	'?'},
/* 0x36	*/	{shift_r_char, shift_r_char},	
/* 0x37 */	{'*',	'*'},    	
/* 0x38 */	{alt_l_char, alt_l_char},
/* 0x39 */	{' ',	' '},		
/* 0x3A */	{caps_lock_char, caps_lock_char}
/*其它按键暂不处理*/
};




static int8_t isempty();
static int8_t isfull();
static int8_t push_last(int8_t value);
static int8_t pop_first();

KEYBOARD_BUFFER kb_buffer;

void handler_keyboard(){
    // put_str("handler-keyboard: ");
    uint8_t data = inb(0x60);
    push_last(data);
    
    // push_last(data);
    // put_hex(data);
    // put_char('\n');
}


void kb_buffer_init(){
    kb_buffer.head = 0;
    kb_buffer.tail = 0;
    kb_buffer.size = 0;
    memory_set(kb_buffer.buffer, KEYBOARD_BUFFER_SIZE, 0);
}


void task_keyboard(){
    while(1){
        if(isempty()){
            continue;
        }
        // ctrl_status = 0, shift_status = 0, alt_status = 0;
        while(kb_buffer.size){
            uint8_t code = pop_first();
            if(code == 0xe0){
                if(isempty()){
                    continue;
                }
                code = pop_first();
                if(code == 0x38){                       // alt
                    alt_status = 1;
                }else if(code == 0x1d){                 // ctrl
                    ctrl_status = 1;
                }
                continue;
            }
            if(code >= 0x3A){
                continue;
            }

            // if((code >= 0x02 && code <= 0x0D) || (code >= 0x10 && code <= 0x1B) ||
            //    (code >= 0x1E && code <= 0x29) || (code >= 0x2B && code <= 0x35) || 
            //    code == 0x37 || code == 0x39 ) {
            //     printf("%c", keymap[code][shift_status]);
            // }

            switch (code){
                case 0x00:
                case 0x01:                                          // esc
                    printf(" you push esc? "); 
                    break; 
                case 0x0E:                                          // backspace
                    printf("%c", backspace);
                    break;
                case 0x0F:                                          // tab
                    printf("%s", "    ");
                    break;
                case 0X1C:                                          // enter
                    printf("\n");
                    break;
                case 0x1D:                                          // ctrl
                    ctrl_status = 1;
                    break;
                case 0x2A:                                          // shift
                case 0x36:
                    printf(" you push shift! ");
                    shift_status = 1;
                    break;
                case 0x38:                                          // alt
                    alt_status = 1;
                    break;
                case 0x3A:
                    printf(" you push caps! ");
                    caps_lock_status = !caps_lock_status;
                    break;
                default:
                    printf("%c", keymap[code][shift_status]);
                    break;
            }
            // ctrl_status = 0, shift_status = 0, alt_status = 0;
        }
        
    }
}













static int8_t isempty(){
    return kb_buffer.size == 0; 
}

static int8_t isfull(){
    return (kb_buffer.tail + 1) % KEYBOARD_BUFFER_SIZE == kb_buffer.head;
}

static int8_t push_last(int8_t value){
    if(isfull()){
        return -1;
    }
    kb_buffer.buffer[kb_buffer.tail] = value;
    kb_buffer.tail = (kb_buffer.tail + 1) % KEYBOARD_BUFFER_SIZE;
    kb_buffer.size++;
    return value;
}

static int8_t pop_first(){
    if(isempty()){
        return -1;
    }
    int8_t value = kb_buffer.buffer[kb_buffer.head];
    kb_buffer.head = (kb_buffer.head + 1) % KEYBOARD_BUFFER_SIZE;
    kb_buffer.size--;
    return value;
}
