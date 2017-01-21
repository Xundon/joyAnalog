#ifndef __HELPERS_H
#define __HELPERS_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"

#define LB_SIZE 256

typedef struct
{
	int32_t last_recv;
	int32_t curr_index;
	char buf[LB_SIZE];
} linear_buf;

int32_t linear_buf_init(linear_buf *lb);
int32_t linear_buf_reset(linear_buf *lb);
int32_t linear_buf_idle(linear_buf *lb);
int32_t linear_buf_line_available(linear_buf *lb);
int32_t linear_buf_add(linear_buf *lb, char c);
int32_t linear_buf_add_str(linear_buf *lb, char *s, uint32_t len);
int32_t get_arg(char* argstr, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif


