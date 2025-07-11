/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <ctype.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

#define DOTS_PER_CELL 6
#define NUM_CELLS 4
#define MAX_INPUT_LEN 100

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} LED;

// Cell 1 pins
LED cell1[DOTS_PER_CELL] = {
    {GPIOA, GPIO_PIN_0},
    {GPIOA, GPIO_PIN_1},
    {GPIOA, GPIO_PIN_4},
    {GPIOB, GPIO_PIN_0},
    {GPIOC, GPIO_PIN_1},
    {GPIOC, GPIO_PIN_0}
};

// Cell 2 pins
LED cell2[DOTS_PER_CELL] = {
    {GPIOA, GPIO_PIN_5},
    {GPIOA, GPIO_PIN_6},
    {GPIOA, GPIO_PIN_7},
    {GPIOB, GPIO_PIN_6},
    {GPIOC, GPIO_PIN_2},
    {GPIOC, GPIO_PIN_3}
};

// Cell 3 pins
LED cell3[DOTS_PER_CELL] = {
    {GPIOB, GPIO_PIN_10},
    {GPIOB, GPIO_PIN_13},
    {GPIOB, GPIO_PIN_14},
    {GPIOB, GPIO_PIN_15},
    {GPIOC, GPIO_PIN_4},
    {GPIOC, GPIO_PIN_5}
};

// Cell 4 pins
LED cell4[DOTS_PER_CELL] = {
    {GPIOA, GPIO_PIN_8},
    {GPIOA, GPIO_PIN_11},
    {GPIOA, GPIO_PIN_12},
    {GPIOA, GPIO_PIN_9},
    {GPIOA, GPIO_PIN_10},
    {GPIOA, GPIO_PIN_15}
};

LED* all_cells[NUM_CELLS] = {cell1, cell2, cell3, cell4};

// Braille map for A-Z (and space at index 26)
const uint8_t braille_map[27][DOTS_PER_CELL] = {
    {1,0,0,0,0,0}, // A
    {1,1,0,0,0,0}, // B
    {1,0,0,1,0,0}, // C
    {1,0,0,1,1,0}, // D
    {1,0,0,0,1,0}, // E
    {1,1,0,1,0,0}, // F
    {1,1,0,1,1,0}, // G
    {1,1,0,0,1,0}, // H
    {0,1,0,1,0,0}, // I
    {0,1,0,1,1,0}, // J
    {1,0,1,0,0,0}, // K
    {1,1,1,0,0,0}, // L
    {1,0,1,1,0,0}, // M
    {1,0,1,1,1,0}, // N
    {1,0,1,0,1,0}, // O
    {1,1,1,1,0,0}, // P
    {1,1,1,1,1,0}, // Q
    {1,1,1,0,1,0}, // R
    {0,1,1,1,0,0}, // S
    {0,1,1,1,1,0}, // T
    {1,0,1,0,0,1}, // U
    {1,1,1,0,0,1}, // V
    {0,1,0,1,1,1}, // W
    {1,0,1,1,0,1}, // X
    {1,0,1,1,1,1}, // Y
    {1,0,1,0,1,1}, // Z
    {0,0,0,0,0,0}  // Space (index 26)
};

/* Function prototypes -------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void Error_Handler(void);

/* Clear all cells */


/* Display character in one Braille cell */
void display_char_in_cell(char ch, LED* cell) {
    uint8_t pattern[DOTS_PER_CELL] = {0};

    if (ch >= 'A' && ch <= 'Z') {
        memcpy(pattern, braille_map[ch - 'A'], DOTS_PER_CELL);
    } else if (ch == ' ') {
        memcpy(pattern, braille_map[26], DOTS_PER_CELL); // Space
    } else {
        memset(pattern, 0, DOTS_PER_CELL); // Unsupported = blank
    }

    for (int i = 0; i < DOTS_PER_CELL; i++) {
        HAL_GPIO_WritePin(cell[i].port, cell[i].pin, pattern[i] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

void clear_all_cells() {
    for (int c = 0; c < NUM_CELLS; c++) {
        for (int i = 0; i < DOTS_PER_CELL; i++) {
            HAL_GPIO_WritePin(all_cells[c][i].port, all_cells[c][i].pin, GPIO_PIN_RESET);
        }
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    uint8_t rx_data[MAX_INPUT_LEN];

    while (1) {
        memset(rx_data, 0, sizeof(rx_data));
        int i = 0;
        HAL_UART_Receive(&huart2, &rx_data[i], 1, HAL_MAX_DELAY);

        while (i < MAX_INPUT_LEN - 1) {
            if (rx_data[i] == '\n' || rx_data[i] == '\r') {
                rx_data[i] = '\0';
                break;
            }
            i++;
            HAL_UART_Receive(&huart2, &rx_data[i], 1, HAL_MAX_DELAY);
        }
        rx_data[i] = '\0';

        int len = strlen((char*)rx_data);
        if (len == 0) continue;

        // 🔁 One letter → one cell at a time (cycling through 4 cells)
        int cell_index = 0;
        for (int j = 0; j < len; j++) {
            clear_all_cells(); // Turn off all LEDs

            char ch = toupper(rx_data[j]);
            display_char_in_cell(ch, all_cells[cell_index]);

            HAL_Delay(1500);  // Delay before moving to next letter

            cell_index = (cell_index + 1) % NUM_CELLS; // Cycle through 0 to 3
        }

        clear_all_cells();  // After full message
    }
}


/* UART2 init function */
static void MX_USART2_UART_Init(void) {
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
}

/* GPIO init function */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    for (int c = 0; c < NUM_CELLS; c++) {
        for (int d = 0; d < DOTS_PER_CELL; d++) {
            GPIO_InitStruct.Pin = all_cells[c][d].pin;
            HAL_GPIO_Init(all_cells[c][d].port, &GPIO_InitStruct);
        }
    }
}

/* System Clock Configuration */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

/* Error Handler */
void Error_Handler(void) {
    while (1) {}
}
