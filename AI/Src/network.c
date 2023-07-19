/**
  ******************************************************************************
  * @file    network.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Mon Jun  5 23:00:15 2023
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */


#include "network.h"
#include "network_data.h"

#include "ai_platform.h"
#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "core_convert.h"

#include "layers.h"

#include <stdio.h>
#include "usart.h"

#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_network
 
#undef AI_NETWORK_MODEL_SIGNATURE
#define AI_NETWORK_MODEL_SIGNATURE     "28d7089f527fec4a331408587e997f34"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     ""
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "Mon Jun  5 23:00:15 2023"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_NETWORK_N_BATCHES
#define AI_NETWORK_N_BATCHES         (1)

static ai_ptr g_network_activations_map[1] = AI_C_ARRAY_INIT;
static ai_ptr g_network_weights_map[1] = AI_C_ARRAY_INIT;



/**  Array declarations section  **********************************************/
/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  node_24_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 6, AI_STATIC)
/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  node_14_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 6, AI_STATIC)
/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  node_14_scratch0_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 320, AI_STATIC)
/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  node_14_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 2, AI_STATIC)
/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  node_17_scratch0_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 240, AI_STATIC)
/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  input_1_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 19200, AI_STATIC)
/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  node_17_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 54, AI_STATIC)
/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  input_1_Transpose_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 19200, AI_STATIC)
/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  node_14_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 3200, AI_STATIC)
/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  node_17_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 3, AI_STATIC)
/* Array#10 */
AI_ARRAY_OBJ_DECLARE(
  node_17_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 1200, AI_STATIC)
/* Array#11 */
AI_ARRAY_OBJ_DECLARE(
  node_20_to_chlast_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 1200, AI_STATIC)
/* Array#12 */
AI_ARRAY_OBJ_DECLARE(
  node_21_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 38400, AI_STATIC)
/* Array#13 */
AI_ARRAY_OBJ_DECLARE(
  node_21_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 32, AI_STATIC)
/* Array#14 */
AI_ARRAY_OBJ_DECLARE(
  node_23_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 32, AI_STATIC)
/* Array#15 */
AI_ARRAY_OBJ_DECLARE(
  node_21_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 32, AI_STATIC)
/* Array#16 */
AI_ARRAY_OBJ_DECLARE(
  node_24_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 6, AI_STATIC)
/* Array#17 */
AI_ARRAY_OBJ_DECLARE(
  node_24_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 192, AI_STATIC)
/**  Tensor declarations section  *********************************************/
/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  node_24_bias, AI_STATIC,
  0, 0x0,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 4, 4, 24, 24),
  1, &node_24_bias_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  node_14_weights, AI_STATIC,
  1, 0x0,
  AI_SHAPE_INIT(4, 3, 1, 1, 2), AI_STRIDE_INIT(4, 4, 12, 12, 12),
  1, &node_14_weights_array, NULL)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  node_14_scratch0, AI_STATIC,
  2, 0x0,
  AI_SHAPE_INIT(4, 1, 2, 80, 2), AI_STRIDE_INIT(4, 4, 4, 8, 640),
  1, &node_14_scratch0_array, NULL)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  node_14_bias, AI_STATIC,
  3, 0x0,
  AI_SHAPE_INIT(4, 1, 2, 1, 1), AI_STRIDE_INIT(4, 4, 4, 8, 8),
  1, &node_14_bias_array, NULL)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  node_17_scratch0, AI_STATIC,
  4, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 40, 2), AI_STRIDE_INIT(4, 4, 4, 12, 480),
  1, &node_17_scratch0_array, NULL)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  input_1_output, AI_STATIC,
  5, 0x0,
  AI_SHAPE_INIT(4, 1, 80, 80, 3), AI_STRIDE_INIT(4, 4, 4, 320, 25600),
  1, &input_1_output_array, NULL)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  node_17_weights, AI_STATIC,
  6, 0x0,
  AI_SHAPE_INIT(4, 2, 3, 3, 3), AI_STRIDE_INIT(4, 4, 8, 24, 72),
  1, &node_17_weights_array, NULL)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  input_1_Transpose_output, AI_STATIC,
  7, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 80, 80), AI_STRIDE_INIT(4, 4, 4, 12, 960),
  1, &input_1_Transpose_output_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  node_14_output, AI_STATIC,
  8, 0x0,
  AI_SHAPE_INIT(4, 1, 2, 40, 40), AI_STRIDE_INIT(4, 4, 4, 8, 320),
  1, &node_14_output_array, NULL)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  node_17_bias, AI_STATIC,
  9, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 4, 4, 12, 12),
  1, &node_17_bias_array, NULL)

/* Tensor #10 */
AI_TENSOR_OBJ_DECLARE(
  node_17_output, AI_STATIC,
  10, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 20, 20), AI_STRIDE_INIT(4, 4, 4, 12, 240),
  1, &node_17_output_array, NULL)

/* Tensor #11 */
AI_TENSOR_OBJ_DECLARE(
  node_20_to_chlast_output, AI_STATIC,
  11, 0x0,
  AI_SHAPE_INIT(4, 1, 20, 20, 3), AI_STRIDE_INIT(4, 4, 4, 80, 1600),
  1, &node_20_to_chlast_output_array, NULL)

/* Tensor #12 */
AI_TENSOR_OBJ_DECLARE(
  node_20_to_chlast_output0, AI_STATIC,
  12, 0x0,
  AI_SHAPE_INIT(4, 1, 1200, 1, 1), AI_STRIDE_INIT(4, 4, 4, 4800, 4800),
  1, &node_20_to_chlast_output_array, NULL)

/* Tensor #13 */
AI_TENSOR_OBJ_DECLARE(
  node_21_weights, AI_STATIC,
  13, 0x0,
  AI_SHAPE_INIT(4, 1200, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4800, 153600, 153600),
  1, &node_21_weights_array, NULL)

/* Tensor #14 */
AI_TENSOR_OBJ_DECLARE(
  node_21_output, AI_STATIC,
  14, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &node_21_output_array, NULL)

/* Tensor #15 */
AI_TENSOR_OBJ_DECLARE(
  node_23_output, AI_STATIC,
  15, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &node_23_output_array, NULL)

/* Tensor #16 */
AI_TENSOR_OBJ_DECLARE(
  node_21_bias, AI_STATIC,
  16, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &node_21_bias_array, NULL)

/* Tensor #17 */
AI_TENSOR_OBJ_DECLARE(
  node_24_output, AI_STATIC,
  17, 0x0,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 4, 4, 24, 24),
  1, &node_24_output_array, NULL)

/* Tensor #18 */
AI_TENSOR_OBJ_DECLARE(
  node_24_weights, AI_STATIC,
  18, 0x0,
  AI_SHAPE_INIT(4, 32, 6, 1, 1), AI_STRIDE_INIT(4, 4, 128, 768, 768),
  1, &node_24_weights_array, NULL)



/**  Layer declarations section  **********************************************/


AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_24_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_23_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_24_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &node_24_weights, &node_24_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_24_layer, 11,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &node_24_chain,
  NULL, &node_24_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_23_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_21_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_23_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_23_layer, 10,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &node_23_chain,
  NULL, &node_24_layer, AI_STATIC, 
  .nl_params = NULL, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_21_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_20_to_chlast_output0),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_21_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &node_21_weights, &node_21_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_21_layer, 9,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &node_21_chain,
  NULL, &node_23_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_20_to_chlast_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_17_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_20_to_chlast_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_20_to_chlast_layer, 7,
  TRANSPOSE_TYPE, 0x0, NULL,
  transpose, forward_transpose,
  &node_20_to_chlast_chain,
  NULL, &node_21_layer, AI_STATIC, 
  .out_mapping = AI_SHAPE_INIT(6, AI_SHAPE_IN_CHANNEL, AI_SHAPE_WIDTH, AI_SHAPE_HEIGHT, AI_SHAPE_CHANNEL, AI_SHAPE_DEPTH, AI_SHAPE_EXTENSION), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_17_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_14_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_17_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &node_17_weights, &node_17_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_17_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  node_17_layer, 6,
  OPTIMIZED_CONV2D_TYPE, 0x0, NULL,
  conv2d_nl_pool, forward_conv2d_nl_pool,
  &node_17_chain,
  NULL, &node_20_to_chlast_layer, AI_STATIC, 
  .groups = 1, 
  .nl_params = NULL, 
  .nl_func = nl_func_relu_array_f32, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_func = pool_func_mp_array_f32, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_14_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &input_1_Transpose_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_14_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &node_14_weights, &node_14_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_14_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  node_14_layer, 3,
  OPTIMIZED_CONV2D_TYPE, 0x0, NULL,
  conv2d_nl_pool, forward_conv2d_nl_pool,
  &node_14_chain,
  NULL, &node_17_layer, AI_STATIC, 
  .groups = 1, 
  .nl_params = NULL, 
  .nl_func = nl_func_relu_array_f32, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_func = pool_func_mp_array_f32, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  input_1_Transpose_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &input_1_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &input_1_Transpose_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  input_1_Transpose_layer, 2,
  TRANSPOSE_TYPE, 0x0, NULL,
  transpose, forward_transpose,
  &input_1_Transpose_chain,
  NULL, &node_14_layer, AI_STATIC, 
  .out_mapping = AI_SHAPE_INIT(6, AI_SHAPE_IN_CHANNEL, AI_SHAPE_HEIGHT, AI_SHAPE_CHANNEL, AI_SHAPE_WIDTH, AI_SHAPE_DEPTH, AI_SHAPE_EXTENSION), 
)


#if (AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 154780, 1, 1),
    154780, NULL, NULL),
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 153600, 1, 1),
    153600, NULL, NULL),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_IN_NUM, &input_1_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_OUT_NUM, &node_24_output),
  &input_1_Transpose_layer, 0, NULL)

#else

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 154780, 1, 1),
      154780, NULL, NULL)
  ),
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 153600, 1, 1),
      153600, NULL, NULL)
  ),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_IN_NUM, &input_1_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_OUT_NUM, &node_24_output),
  &input_1_Transpose_layer, 0, NULL)

#endif	/*(AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)*/


/******************************************************************************/
AI_DECLARE_STATIC
ai_bool network_configure_activations(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_activations_map(g_network_activations_map, 1, params)) {
    /* Updating activations (byte) offsets */
    
    input_1_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    input_1_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    input_1_Transpose_output_array.data = AI_PTR(g_network_activations_map[0] + 76800);
    input_1_Transpose_output_array.data_start = AI_PTR(g_network_activations_map[0] + 76800);
    
    node_14_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 0);
    node_14_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    node_14_output_array.data = AI_PTR(g_network_activations_map[0] + 1280);
    node_14_output_array.data_start = AI_PTR(g_network_activations_map[0] + 1280);
    
    node_17_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 0);
    node_17_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    node_17_output_array.data = AI_PTR(g_network_activations_map[0] + 14080);
    node_17_output_array.data_start = AI_PTR(g_network_activations_map[0] + 14080);
    
    node_20_to_chlast_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    node_20_to_chlast_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    node_21_output_array.data = AI_PTR(g_network_activations_map[0] + 4800);
    node_21_output_array.data_start = AI_PTR(g_network_activations_map[0] + 4800);
    
    node_23_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    node_23_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    node_24_output_array.data = AI_PTR(g_network_activations_map[0] + 128);
    node_24_output_array.data_start = AI_PTR(g_network_activations_map[0] + 128);
    
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_ACTIVATIONS);
  return false;
}



/******************************************************************************/
AI_DECLARE_STATIC
ai_bool network_configure_weights(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_weights_map(g_network_weights_map, 1, params)) {
    /* Updating weights (byte) offsets */
    
    node_24_bias_array.format |= AI_FMT_FLAG_CONST;
    node_24_bias_array.data = AI_PTR(g_network_weights_map[0] + 0);
    node_24_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 0);
    
    node_14_weights_array.format |= AI_FMT_FLAG_CONST;
    node_14_weights_array.data = AI_PTR(g_network_weights_map[0] + 24);
    node_14_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 24);
    
    node_14_bias_array.format |= AI_FMT_FLAG_CONST;
    node_14_bias_array.data = AI_PTR(g_network_weights_map[0] + 48);
    node_14_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 48);
    
    node_17_weights_array.format |= AI_FMT_FLAG_CONST;
    node_17_weights_array.data = AI_PTR(g_network_weights_map[0] + 56);
    node_17_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 56);
    
    node_17_bias_array.format |= AI_FMT_FLAG_CONST;
    node_17_bias_array.data = AI_PTR(g_network_weights_map[0] + 272);
    node_17_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 272);
    
    node_21_weights_array.format |= AI_FMT_FLAG_CONST;
    node_21_weights_array.data = AI_PTR(g_network_weights_map[0] + 284);
    node_21_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 284);
    
    node_21_bias_array.format |= AI_FMT_FLAG_CONST;
    node_21_bias_array.data = AI_PTR(g_network_weights_map[0] + 153884);
    node_21_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 153884);
    
    node_24_weights_array.format |= AI_FMT_FLAG_CONST;
    node_24_weights_array.data = AI_PTR(g_network_weights_map[0] + 154012);
    node_24_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 154012);
    
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_WEIGHTS);
  return false;
}


/**  PUBLIC APIs SECTION  *****************************************************/


AI_DEPRECATED
AI_API_ENTRY
ai_bool ai_network_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_MODEL_NAME,
      .model_signature   = AI_NETWORK_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 198667,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .params            = AI_STRUCT_INIT,
      .activations       = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x0,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}


AI_API_ENTRY
ai_bool ai_network_get_report(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_MODEL_NAME,
      .model_signature   = AI_NETWORK_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 198667,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .map_signature     = AI_MAGIC_SIGNATURE,
      .map_weights       = AI_STRUCT_INIT,
      .map_activations   = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x0,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}

AI_API_ENTRY
ai_error ai_network_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}

AI_API_ENTRY
ai_error ai_network_create(
  ai_handle* network, const ai_buffer* network_config)
{
  char Tx_buf[50];
  int Tx_buf_len = 0;
	Tx_buf_len = sprintf(Tx_buf, "\r\nAI - initialization3\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
  return ai_platform_network_create(
    network, network_config, 
    &AI_NET_OBJ_INSTANCE,
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}

AI_API_ENTRY
ai_error ai_network_create_and_init(
  ai_handle* network, const ai_handle activations[], const ai_handle weights[])
{
    ai_error err;
    ai_network_params params;
    char Tx_buf[50];
    int Tx_buf_len = 0;
	  Tx_buf_len = sprintf(Tx_buf, "\r\nAI - initialization2\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
    err = ai_network_create(network, AI_NETWORK_DATA_CONFIG);
    if (err.type != AI_ERROR_NONE)
        return err;
    if (ai_network_data_params_get(&params) != true) {
        err = ai_network_get_error(*network);
        return err;
    }
#if defined(AI_NETWORK_DATA_ACTIVATIONS_COUNT)
    if (activations) {
        /* set the addresses of the activations buffers */
        for (int idx=0;idx<params.map_activations.size;idx++)
            AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_activations, idx, activations[idx]);
    }
#endif
#if defined(AI_NETWORK_DATA_WEIGHTS_COUNT)
    if (weights) {
        /* set the addresses of the weight buffers */
        for (int idx=0;idx<params.map_weights.size;idx++)
            AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_weights, idx, weights[idx]);
    }
#endif
    if (ai_network_init(*network, &params) != true) {
        err = ai_network_get_error(*network);
    }
    return err;
}

AI_API_ENTRY
ai_buffer* ai_network_inputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    ((ai_network *)network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_inputs_get(network, n_buffer);
}

AI_API_ENTRY
ai_buffer* ai_network_outputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    ((ai_network *)network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_outputs_get(network, n_buffer);
}

AI_API_ENTRY
ai_handle ai_network_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}

AI_API_ENTRY
ai_bool ai_network_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = ai_platform_network_init(network, params);
  if (!net_ctx) return false;

  ai_bool ok = true;
  ok &= network_configure_weights(net_ctx, params);
  ok &= network_configure_activations(net_ctx, params);

  ok &= ai_platform_network_post_init(network);

  return ok;
}


AI_API_ENTRY
ai_i32 ai_network_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}

AI_API_ENTRY
ai_i32 ai_network_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}



#undef AI_NETWORK_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME

