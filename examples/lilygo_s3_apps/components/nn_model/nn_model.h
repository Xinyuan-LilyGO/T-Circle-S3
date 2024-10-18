#ifndef _NN_MODEL_H_
#define _NN_MODEL_H_

#include <cstring>
#include <stdio.h>
#include <stdlib.h>

typedef void *nn_model_handle_t;

struct nn_model_config_t {
  const unsigned char *model_ptr;
  const char **labels;
  unsigned int labels_num;
  bool is_quantized;
  float inference_threshold;
};

/*!
 * \brief Initialize NN model.
 * \param model_handle NN model handle.
 * \param cfg NN model config.
 * \return Result.
 */
int nn_model_init(nn_model_handle_t *model_handle, nn_model_config_t cfg);
/*!
 * \brief Release model.
 * \param model_handle NN model handle.
 * \return Result.
 */
int nn_model_release(nn_model_handle_t model_handle);
/*!
 * \brief Model inference.
 * \param model_handle NN model handle.
 * \param input_data input data.
 * \param len input data len.
 * \param category inferred category.
 * \return Result.
 */
int nn_model_inference(nn_model_handle_t model_handle, const float *input_data,
                       size_t len, int *category);
/*!
 * \brief Get label string.
 * \param model_handle NN model handle.
 * \param category Category index.
 * \param buffer Buffer to store label.
 * \param len Buffer len.
 * \return Result.
 */
int nn_model_get_label(nn_model_handle_t model_handle, int category,
                       char *buffer, size_t len);

#endif // _NN_MODEL_H_
