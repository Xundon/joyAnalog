#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "helpers.h"
#include "shared.h"
#include "cmd_parser.h"
#include "eeprom.h"

#define ARG_PARSE_SUCCESS 0
#define ARG_PARSE_ERROR_INVALID_CMD 126
#define ARG_PARSE_ERROR_NOT_AVAILABLE 127
#define SWITCH_JOYCON_BUTTON_COUNT 24

char* goto_next_arg(char* buf)
{
  char* curr = buf;
  if(curr == NULL)
    return NULL;
  char* buf_end = curr + strlen(curr);
  if(curr >= buf_end)
    return NULL;
  while(curr < buf_end && *curr != ' ')
      curr++;
  while(curr < buf_end && *curr == ' ')
      curr++;
  if(curr >= buf_end)
    return NULL;
  return curr;
}

int32_t arg_to_button_index(char* cmd)
{
  int32_t result;
  if(strncmp(cmd, "dd", 2) == 0)
    result = 8;
  else if(strncmp(cmd, "du", 2) == 0)
    result = 9;
  else if(strncmp(cmd, "dr", 2) == 0)
    result = 10;
  else if(strncmp(cmd, "dl", 2) == 0)
    result = 11;
  else if(strncmp(cmd, "lsr", 3) == 0)
    result = 12;
  else if(strncmp(cmd, "lsl", 3) == 0)
    result = 13;
  else if(strncmp(cmd, "ls", 2) == 0)
    result = 14;
  else if(strncmp(cmd, "zl", 2) == 0)
    result = 15;
  else if(strncmp(cmd, "-", 1) == 0)
    result = 4;
  else if(strncmp(cmd, "sbl", 3) == 0)
    result = 5;
  else if(strncmp(cmd, "cap", 3) == 0)
    result = 6;
  else if(strncmp(cmd, "syncl", 5) == 0)
    result = 7;
  
  else if(strncmp(cmd, "y", 1) == 0)
    result = 8 + 12;
  else if(strncmp(cmd, "x", 1) == 0)
    result = 9 + 12;
  else if(strncmp(cmd, "b", 1) == 0)
    result = 10 + 12;
  else if(strncmp(cmd, "a", 1) == 0)
    result = 11 + 12;
  else if(strncmp(cmd, "rsr", 3) == 0)
    result = 12 + 12;
  else if(strncmp(cmd, "rsl", 3) == 0)
    result = 13 + 12;
  else if(strncmp(cmd, "rs", 2) == 0)
    result = 14 + 12;
  else if(strncmp(cmd, "zr", 2) == 0)
    result = 15 + 12;
  else if(strncmp(cmd, "+", 1) == 0)
    result = 4 + 12;
  else if(strncmp(cmd, "sbr", 3) == 0)
    result = 5 + 12;
  else if(strncmp(cmd, "h", 1) == 0)
    result = 6 + 12;
  else if(strncmp(cmd, "syncr", 5) == 0)
    result = 7 + 12;
  else
    return ARG_PARSE_ERROR_INVALID_CMD;

  if((board_type == BOARD_TYPE_NDAC_MINI_JOYCON_LEFT && result >= 4 && result <= 15) ||
    (board_type == BOARD_TYPE_NDAC_MINI_JOYCON_RIGHT && result >= 16 && result <= 27))
    return result;
  return ARG_PARSE_ERROR_NOT_AVAILABLE;
}

int32_t process_multiarg(char* args)
{
  char* arg_ptr = args;
  int32_t count = 0;
  int32_t result;
  while(1)
  {
    if(arg_ptr == NULL || count >= ARG_QUEUE_SIZE)
      break;
    result = arg_to_button_index(arg_ptr);
    if(result >= 4 && result <= 27)
    {
      count++;
      // set corresponding bit to 1
    }
    else
      return result;
    arg_ptr = goto_next_arg(arg_ptr);
  }
  if(count <= 0)
    return ARG_PARSE_ERROR_INVALID_CMD;
  return ARG_PARSE_SUCCESS;
}

void button_ctrl(GPIO_PinState action)
{
  for(int i = 0; i < ARG_QUEUE_SIZE; ++i)
    if(gpio_port_queue[i] != NULL)
      HAL_GPIO_WritePin(gpio_port_queue[i], gpio_pin_queue[i], action);
}

int32_t button_hold(char* cmd)
{
  char* arg_start = goto_next_arg(cmd);
  int32_t result = process_multiarg(arg_start);
  if(result == ARG_PARSE_SUCCESS)
    button_ctrl(GPIO_PIN_RESET);
  return result;
}

int32_t button_release(char* cmd)
{
  char* arg_start = goto_next_arg(cmd);
  int32_t result = process_multiarg(arg_start);
  if(result == ARG_PARSE_SUCCESS)
    button_ctrl(GPIO_PIN_SET);
  return result;
}

int32_t stick_hold(char* cmd)
{
  char* x_ptr = goto_next_arg(cmd);
  char* y_ptr = goto_next_arg(x_ptr);
  uint32_t x_8b = atoi(x_ptr);
  uint32_t y_8b = atoi(y_ptr);
  uint32_t x_12b = (uint32_t)((float)x_8b * 8.76);
  uint32_t y_12b = (uint32_t)((float)y_8b * 8.76);
  if(x_ptr == NULL || y_ptr == NULL || x_8b > 255 || y_8b > 255)
    return ARG_PARSE_ERROR_INVALID_CMD;
  if(stm32_dac_ptr->State == HAL_DAC_STATE_RESET)
    stm32_dac_init();
  HAL_DACEx_DualSetValue(stm32_dac_ptr, DAC_ALIGN_12B_R, x_12b, y_12b);
  return ARG_PARSE_SUCCESS;
}

void stick_release(void)
{
  if(stm32_dac_ptr->State == HAL_DAC_STATE_RESET)
    stm32_dac_init();
  HAL_DACEx_DualSetValue(stm32_dac_ptr, DAC_ALIGN_12B_R, 1117, 1117);
}

void stick_disengage(void)
{
  if(stm32_dac_ptr->State != HAL_DAC_STATE_RESET)
  {
    HAL_DAC_Stop(stm32_dac_ptr, DAC_CHANNEL_1);
    HAL_DAC_Stop(stm32_dac_ptr, DAC_CHANNEL_2);
    HAL_DAC_DeInit(stm32_dac_ptr);
  }
}

void release_all_button(void)
{
  for(int i = 0; i < SWITCH_JOYCON_BUTTON_COUNT; ++i)
      HAL_GPIO_WritePin(gpio_port_map[i], gpio_pin_map[i], GPIO_PIN_SET);
}

void parse_cmd(char* cmd)
{
  int32_t result;
  if(strcmp(cmd, "test") == 0)
    puts("test OK");
  if(strcmp(cmd, "eepinit") == 0)
  {
    eeprom_erase();
    eeprom_write(EEPROM_BOARD_TYPE_ADDR, BOARD_TYPE_NDAC_MINI_JOYCON_LEFT);
    board_type = BOARD_TYPE_NDAC_MINI_JOYCON_LEFT;
    puts("eepinit OK");
  }
  else if(strncmp(cmd, "settype l", 9) == 0)
  {
    eeprom_write(EEPROM_BOARD_TYPE_ADDR, BOARD_TYPE_NDAC_MINI_JOYCON_LEFT);
    board_type = BOARD_TYPE_NDAC_MINI_JOYCON_LEFT;
    printf("settype OK\n");
  }
  else if(strncmp(cmd, "settype r", 9) == 0)
  {
    eeprom_write(EEPROM_BOARD_TYPE_ADDR, BOARD_TYPE_NDAC_MINI_JOYCON_RIGHT);
    board_type = BOARD_TYPE_NDAC_MINI_JOYCON_RIGHT;
    printf("settype OK\n");
  }
  else if(strcmp(cmd, "whoami") == 0)
  {
    board_type = eeprom_read(EEPROM_BOARD_TYPE_ADDR);
    switch(board_type)
    {
      case BOARD_TYPE_NDAC_MINI_JOYCON_LEFT:
      puts("BOARD_TYPE_NDAC_MINI_JOYCON_LEFT");
      break;

      case BOARD_TYPE_NDAC_MINI_JOYCON_RIGHT:
      puts("BOARD_TYPE_NDAC_MINI_JOYCON_RIGHT");
      break;

      default:
      puts("unknown board type, use 'settype l/r' to configure this board");
      break;
    }
  }
  // button hold, multiple args allowed
  else if(strncmp(cmd, "bh ", 3) == 0)
  {
    result = button_hold(cmd);
    switch(result)
    {
      case ARG_PARSE_SUCCESS:
      puts("bh OK");
      break;
      case ARG_PARSE_ERROR_INVALID_CMD:
      puts("bh ERROR: invalid command");
      break;
      case ARG_PARSE_ERROR_NOT_AVAILABLE:
      puts("bh ERROR: not available on this side");
      break;
      default:
      puts("bh ERROR: unknown");
    }
  }
  // button release, multiple args allowed
  else if(strncmp(cmd, "br ", 3) == 0)
  {
    result = button_release(cmd);
    switch(result)
    {
      case ARG_PARSE_SUCCESS:
      puts("br OK");
      break;
      case ARG_PARSE_ERROR_INVALID_CMD:
      puts("br ERROR: invalid command");
      break;
      case ARG_PARSE_ERROR_NOT_AVAILABLE:
      puts("br ERROR: not available on this side");
      break;
      default:
      puts("br ERROR: unknown");
    }
  }
  // button release all
  else if(strcmp(cmd, "bra") == 0)
  {
    release_all_button();
    puts("bra OK");
  }
  // stick hold, sh x y, x and y between 0 and 255 inclusive
  else if(strncmp(cmd, "sh ", 3) == 0)
  {
    result = stick_hold(cmd);
    switch(result)
    {
      case ARG_PARSE_SUCCESS:
      puts("sh OK");
      break;
      case ARG_PARSE_ERROR_INVALID_CMD:
      puts("sh ERROR: invalid command");
      break;
      default:
      puts("sh ERROR: unknown");
    }
  }
  // stick release, to netural position 
  else if(strcmp(cmd, "sr") == 0)
  {
    stick_release();
    puts("sr OK");
  }
  // stick disengage, gives control back to user
  else if(strcmp(cmd, "sd") == 0)
  {
    stick_disengage();
    puts("sd OK");
  }
  // release all buttons and sticks
  else if(strcmp(cmd, "reset") == 0)
  {
    release_all_button();
    stick_disengage();
    puts("reset OK");
  }
  else
  {
    puts("ERROR unknown command");
  }
}