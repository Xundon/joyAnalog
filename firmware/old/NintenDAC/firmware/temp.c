 switch(result)
  {
    case ARG_PARSE_SUCCESS:
    button_ctrl(GPIO_PIN_RESET);
    puts("bh OK");
    break;

    case ARG_PARSE_ERROR_INVALID_CMD:
    puts("bh ERROR: invalid command");
    break;

    case ARG_PARSE_ERROR_NOT_AVAILABLE:
    puts("bh ERROR: button not available");
    break;

    default:
    puts("bh ERROR: unknown");
  }


  int32_t get_arg(char* argstr, uint32_t index)
{
  if(argstr == NULL)
    return INT32MAX;

  char* str_end = argstr + strlen(argstr);
  char* curr = argstr;
  int32_t count = 0;

  while(curr < str_end && *curr != ' ')
    curr++;
  curr++;
  if(curr >= str_end)
    return INT32MAX - 1;
  if(index == 0)
    return atoi(curr);

  while(curr < str_end)
  {
    if(*curr == ' ')
      count++;
    if(count == index)
      break;
    curr++;
  }
  curr++;
  if(curr >= str_end)
    return INT32MAX - 2;
  return atoi(curr);
}


  // built in dac starts in high-z mode
  HAL_DAC_DeInit(stm32_dac_ptr);
  printf("%d, %d\n", x_12b, y_12b);

  else if(strncmp(cmd, "sd", 2) == 0)
  {
    HAL_DAC_DeInit(stm32_dac_ptr);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = STM32_DAC_X_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(STM32_DAC_X_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = STM32_DAC_Y_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(STM32_DAC_Y_GPIO_Port, &GPIO_InitStruct);

    puts("sd OK");
  }
void button_ctrl(int32_t action)
{
  for(int i = 0; i < ARG_QUEUE_SIZE; ++i)
  {
    if(gpio_port_queue[i] != NULL)
    {
      printf("acting on %d...\n", i);
      if(action == ACTION_HOLD)
        HAL_GPIO_WritePin(gpio_port_queue[i], gpio_pin_queue[i], GPIO_PIN_RESET);
      else
        HAL_GPIO_WritePin(gpio_port_queue[i], gpio_pin_queue[i], GPIO_PIN_SET);
    }
  }
}

int32_t endswith(char* source, char* query)
{
  return strncmp(source, query, 4) == 0 && strlen(source) == strlen(query);
}

    for (int i = 0; i < ARG_QUEUE_SIZE; ++i)
      printf("%d: %d\n", i, gpio_pin_queue[i]);
void release_all_buttons(void)
{
  puts("bra ok");
}


  char* my_usb_readline(void)
{
  char* ret = NULL;
  if(usb_recv_buf.curr_index > 0)
  {
    if(usb_recv_buf.buf[usb_recv_buf.curr_index - 1] == '\n')
    {
      for (int i = 0; i < usb_recv_buf.curr_index; ++i)
        if(usb_recv_buf.buf[i] == '\n' || usb_recv_buf.buf[i] == '\r')
          usb_recv_buf.buf[i] = 0;
      memset(usb_line_buf, 0, LB_SIZE);
      strcpy(usb_line_buf, usb_recv_buf.buf);
      ret = usb_line_buf;
      linear_buf_reset(&usb_recv_buf);
    }
    else if(HAL_GetTick() - usb_recv_buf.last_recv > 250)
      linear_buf_reset(&usb_recv_buf);
  }
  return ret;
}




  if(strncmp(cmd, "test\n", 5) == 0)
    printf("im alive!\n");
  for (int i = 0; i < strlen(cmd); ++i)
    printf("%c %d\n", cmd[i], cmd[i]);
void parse_cmd(char* cmd)
{
  if(cmd == NULL)
    return;

  if(strncmp(cmd, "test\n", 5) == 0)
    puts("test OK");
  else if(strncmp(cmd, "bh\n", 3) == 0)
  {
    puts("bh OK");
    HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, GPIO_PIN_RESET);
  }
  else if(strncmp(cmd, "br\n", 3) == 0)
  {
    puts("br OK");
    HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, GPIO_PIN_SET);
  }
  else
    puts("ERROR");
}

        usb_line_buf[strlen(usb_line_buf)] = '\n';


  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    usb_data = my_usb_readline();
    if(usb_data != NULL)
    {
      // printf("usb_lb: %s\n", usb_data);
      // stm_dac_test();
      serial_test_flag = (serial_test_flag + 1) % 2;
      printf("serial_test: %d\n", serial_test_flag);
    }
    if(serial_test_flag)
      my_usb_putchar('k');
  }


HAL_DAC_DeInit(stm32_dac_ptr);

// max dac
void test(void)
{
  max572x_POWER(0x3, max572x_POWER_NORMAL);
  printf("run started\n");
  uint16_t count = 0;
  while(1)
  {
    max572x_CODEn_LOADn(0, count);
    max572x_CODEn_LOADn(1, ~count);
    count++;
  }
}

// bulit in dac
void test(void)
{
  printf("run started\n");
  uint16_t count = 0;
  while(1)
  {
    // HAL_DAC_SetValue(stm32_dac_ptr, DAC_CHANNEL_1, DAC_ALIGN_8B_R, count);
    // HAL_DACEx_DualSetValue(stm32_dac_ptr, DAC_ALIGN_8B_R, count, 255 - count);
    HAL_DACEx_DualSetValue(stm32_dac_ptr, DAC_ALIGN_12B_R, count, 65535 - count);
    count++;
  }
}


if(linear_buf_line_available(&debug_lb))
		{
		  printf("debug_lb: %s\n", debug_lb.buf);

		  if(strstr(debug_lb.buf, "s ") != NULL)
		  {
		  	uint8_t to_send = get_arg(debug_lb.buf, 0);
		  	uint8_t received = 0;
		  	printf("sending %d...\n", to_send);
		  	spi_cs_low();
		  	HAL_SPI_TransmitReceive(max572x_spi_ptr, &to_send, &received, 1, 500);
		  	spi_cs_high();
		  	printf("received: %d\n", received);
		  }
		  linear_buf_reset(&debug_lb);
		}


  SPI2_CS_GPIO_Port->ODR |= 0x1000; // CS high
  SPI2_CS_GPIO_Port->ODR &= 0xefff; // CS low


void max572x_POWER(uint8_t dac_multi_sel, uint8_t power_mode)
{
  uint8_t power = (uint8_t)((power_mode << 6) & 0xc0);
  printf("power: 0x%X\n", power);
  // uint8_t to_send[3] = {0x40, dac_multi_sel, (power_mode << 6) & 0xc0};
}

  printf("sending 0x%X 0x%X 0x%X...\n", to_send[0], to_send[1], to_send[2]);
void max572x_POWER(uint8_t dac_multi_sel, uint8_t power_mode)
{
  uint8_t to_send[3] = {0x40, dac_multi_sel, (uint8_t)((power_mode << 6) & 0xc0)};
  spi_cs_low();
  HAL_SPI_Transmit(max572x_spi_ptr, to_send, 3, 100);
  spi_cs_high();
}

void read_status()
{
  uint8_t spi_sb[3] = {0xe8, 0xaa, 0xaa};
  uint8_t spi_rb[3] = {0, 0, 0};
  printf("sending 0x%X 0x%X 0x%X...\n", spi_sb[0], spi_sb[1], spi_sb[2]);
  spi_cs_low();
  HAL_SPI_TransmitReceive(max572x_spi_ptr, spi_sb, spi_rb, 3, 500);
  spi_cs_high();
  printf("spi_rb: 0x%X 0x%X 0x%X\n", spi_rb[0], spi_rb[1], spi_rb[2]);
}

void read_status()
{
  uint8_t spi_sb[3] = {0xe8, 0xaa, 0xaa};
  uint8_t spi_rb[3] = {0, 0, 0};
  printf("sending: ");
  print_3b(spi_sb);
  spi_cs_low();
  HAL_SPI_TransmitReceive(max572x_spi_ptr, spi_sb, spi_rb, 3, 500);
  spi_cs_high();
  printf("received: ");
  print_3b(spi_rb);
}

void max5725_CODEn(uint8_t dac_sel, uint16_t data)
{
  uint8_t spi_sb[3] = {0x80, 0, 0};
  dac_sel &= 0xf;
  spi_sb[0] |= dac_sel;
  data &= 0xfff;
  spi_sb[1] |= data >> 4;
  data &= 0xf;
  spi_sb[2] |= (data << 4);
  spi_cs_low();
  HAL_SPI_Transmit(max572x_spi_ptr, spi_sb, 3, 100);
  spi_cs_high();
  // printf("max5725_CODEn: "); print_3b(spi_sb);
}

void max5723_CODEn(uint8_t dac_sel, uint8_t data)
{
  uint8_t spi_sb[3] = {0x80, 0, 0};
  dac_sel &= 0xf;
  spi_sb[0] |= dac_sel;
  spi_sb[1] = data;
  spi_cs_low();
  HAL_SPI_Transmit(max572x_spi_ptr, spi_sb, 3, 100);
  spi_cs_high();
  // printf("max5723_CODEn: "); print_3b(spi_sb);
}

void pack_dac_val(uint8_t part, uint16_t value, uint8_t* dh, uint8_t* dl)
{
  if(part == MAX5723)
    *dh = (value & 0xff);
  else if(part == MAX5724)
  {
    value &= 0x3ff;
    *dh |= value >> 2;
    value &= 0x3;
    *dl |= (value << 6);
  }
  else
  {
    value &= 0xfff;
    *dh |= value >> 4;
    value &= 0xf;
    *dl |= (value << 4);
  }
}


void max5723_CODEn(uint8_t dac_sel, uint8_t data)
{
  uint8_t spi_sb[3] = {0x80, 0, 0};
  dac_sel &= 0xf;
  spi_sb[0] |= dac_sel;
  spi_sb[1] = data;
  spi_cs_low();
  HAL_SPI_Transmit(max572x_spi_ptr, spi_sb, 3, 100);
  spi_cs_high();
  // printf("max5723_CODEn: "); print_3b(spi_sb);
}

void max5724_CODEn(uint8_t dac_sel, uint16_t data)
{
  uint8_t spi_sb[3] = {0x80, 0, 0};
  dac_sel &= 0xf;
  spi_sb[0] |= dac_sel;
  data &= 0x3ff;
  spi_sb[1] |= data >> 2;
  data &= 0x3;
  spi_sb[2] |= (data << 6);
  spi_cs_low();
  HAL_SPI_Transmit(max572x_spi_ptr, spi_sb, 3, 100);
  spi_cs_high();
  // printf("max5724_CODEn: "); print_3b(spi_sb);
}

void max5725_CODEn(uint8_t dac_sel, uint16_t data)
{
  uint8_t spi_sb[3] = {0x80, 0, 0};
  dac_sel &= 0xf;
  spi_sb[0] |= dac_sel;
  data &= 0xfff;
  spi_sb[1] |= data >> 4;
  data &= 0xf;
  spi_sb[2] |= (data << 4);
  spi_cs_low();
  HAL_SPI_Transmit(max572x_spi_ptr, spi_sb, 3, 100);
  spi_cs_high();
  // printf("max5725_CODEn: "); print_3b(spi_sb);
}

// printf("max572x_SW_RESET\n");
// printf("max572x_POWER: "); print_3b(spi_sb);
// printf("max572x_CONFIG: "); print_3b(spi_sb);
// printf("max572x_WDOG: "); print_3b(spi_sb);
// printf("max572x_REF: "); print_3b(spi_sb);
// printf("max572x_CODEn: "); print_3b(spi_sb);
// printf("max572x_LOADn: "); print_3b(spi_sb);

void print_3b(uint8_t buf[3])
{
  printf("0x%x 0x%x 0x%x\n", buf[0], buf[1], buf[2]);
}

#define MAX5723 0
#define MAX5724 1
#define MAX5725 2

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (unsigned char *)&ch, 1, 100);
    return ch;
}
while (1)
  {
    HAL_UART_Receive_IT(&huart1, debug_byte_buf, 1);
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    if(linear_buf_line_available(&debug_lb))
    {
      printf("debug_lb: %s\n", debug_lb.buf);
      test();
      linear_buf_reset(&debug_lb);
    }
  }

  while(1)
  {
    usb_data = my_usb_readline();
    if(usb_data != NULL)
    {
      printf("I received: %s\n", usb_data);
    }
  }

uint8_t debug_byte_buf[1];
linear_buf debug_lb;
extern UART_HandleTypeDef huart1;
#define debug_uart_ptr (&huart1)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance==USART1)
  {
      linear_buf_add(&debug_lb, debug_byte_buf[0]);
      HAL_UART_Receive_IT(&huart1, debug_byte_buf, 1);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance==USART1)
  {
      HAL_UART_Receive_IT(&huart1, debug_byte_buf, 1);
      linear_buf_reset(&debug_lb);
  }
}
      HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);