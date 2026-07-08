/*
 * nlms_filter.h
 *
 * Created on: May 1, 2026
 * Author: giova (Convertido para C)
 */

#ifndef APP_FILTER_NLMSFILTER_H_
#define APP_FILTER_NLMSFILTER_H_

#include <stddef.h>

/* Um filtro adaptativo tem o formato de um regressor linear multidimensional FIR (como ilustrado abaixo)
 *
 * Y(n) = w0*u(n) + w1*u(n-1) + ... + w(N-1)*u(n-N+1) + w(N)*u(n-N)
 * sendo Y(n) a saída estimada pelo filtro
 * w(i) a matriz de pesos do filtro
 * u(n) a N-ésima entrada do filtro (do presente n para o passado N)
 * N tamanho do buffer/quuantidade de amostras passadas/ Ordem do filtro
 */

typedef float nlms_type_t;

typedef struct {
    nlms_type_t *weights;    // Matriz de pesos do filtro (w) - fornecida pelo usuário
    nlms_type_t *delay_line; // Buffer de entrada (u) - fornecida pelo usuário
    size_t N;                // Ordem do filtro (equivalente ao <size_t N> do template)

    nlms_type_t mu;          // Taxa de Aprendizado (igual neurônio artificial)
    nlms_type_t delta;       // Fator de Regularização (para não dividir por zero)

    nlms_type_t last_error;
    nlms_type_t last_output;

    size_t head_index;       // indexador do buffer circular
} NLMSFilter;

/**
 * @brief Inicializa a estrutura do Filtro NLMS
 * @param filter Ponteiro para a estrutura do filtro
 * @param N Ordem do filtro
 * @param weights_buffer Array pre-alocado para os pesos (tamanho N)
 * @param delay_line_buffer Array pre-alocado para o atraso (tamanho N)
 * @param learning_rate Passo de adaptação (dentre 0 e 1)
 * @param regularization Fator para evitar divisão por zero
 */
void NLMSFilter_Init(NLMSFilter *filter, size_t N, nlms_type_t *weights_buffer, nlms_type_t *delay_line_buffer, nlms_type_t learning_rate, nlms_type_t regularization);

/**
 * @brief Processa a amostra e atualiza os pesos adaptativos
 * @param filter Ponteiro para a estrutura do filtro
 * @param input_sample Amostra inserida na linha de atraso
 * @param desired_sample Valor desejado (referência de erro)
 * @return O valor estimado/filtrado (y)
 */
nlms_type_t NLMSFilter_Update(NLMSFilter *filter, nlms_type_t input_sample, nlms_type_t desired_sample);

/**
 * @brief Reinicia os pesos e zera os buffers
 * @param filter Ponteiro para a estrutura do filtro
 * @param default_weight Valor inicial para os pesos
 * @param default_buffer_value Valor inicial para o buffer de entrada
 */
void NLMSFilter_Reset(NLMSFilter *filter, nlms_type_t default_weight, nlms_type_t default_buffer_value);

// ==========================================
// Setters para calibração
// ==========================================
void NLMSFilter_SetLearningRate(NLMSFilter *filter, nlms_type_t new_mu);
void NLMSFilter_SetRegularization(NLMSFilter *filter, nlms_type_t new_delta);

// ==========================================
// Getters para monitoramento / Filtro Casado
// ==========================================
nlms_type_t NLMSFilter_GetLastError(const NLMSFilter *filter);
nlms_type_t NLMSFilter_GetLastOutput(const NLMSFilter *filter);

/**
 * @brief Retorna o buffer ordenado cronologicamente
 * @param filter Ponteiro para a estrutura do filtro
 * @param ordered_out Array de saída (tamanho N) pre-alocado pelo usuário
 */
void NLMSFilter_GetOrderedBuffer(const NLMSFilter *filter, nlms_type_t *ordered_out);

#endif /* APP_FILTER_NLMSFILTER_H_ */