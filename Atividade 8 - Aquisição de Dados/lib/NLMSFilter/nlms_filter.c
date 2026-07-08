/*
 * nlms_filter.c
 */

#include "nlms_filter.h"

// Inicialização do filtro
void NLMSFilter_Init(NLMSFilter *filter, size_t N, nlms_type_t *weights_buffer, nlms_type_t *delay_line_buffer, nlms_type_t learning_rate, nlms_type_t regularization)
{
    filter->weights = weights_buffer;
    filter->delay_line = delay_line_buffer;
    filter->N = N;
    filter->mu = learning_rate;
    filter->delta = regularization;
    filter->last_error = 0.0f;
    filter->last_output = 0.0f;
    filter->head_index = 0;

    // Pesos com valor 0 inicialmente
    NLMSFilter_Reset(filter, 0.0f, 0.0f);
}

// Atualização dos pesos do filtro, retorna y
// input_sample = entrada atual
// desired_sample = saida alvo (ideal)
nlms_type_t NLMSFilter_Update(NLMSFilter *filter, nlms_type_t input_sample, nlms_type_t desired_sample)
{
    //% Atualiza o vetor de entrada, inserindo o novo dado na pilha
    filter->delay_line[filter->head_index] = input_sample;

    nlms_type_t y = 0.0f;        // Saida que será estimada
    nlms_type_t norm_sq = 0.0f;  // Normalizacao da entrada

    // Faz duas coisas nesse laço;
    // 1-Calcula o valor da saida com base nos pesos:
    // Y(n) = Wo*u(n) + ... + WN*u(n-N)
    // 2-A Energia do valor de entrada
    // E = u(n)*u(n)
    for (size_t i = 0; i < filter->N; i++)
    {
        // Implementa a lógica do Buffer Circular para o indexador
        size_t idx = (filter->head_index + filter->N - i) % filter->N;
        // Pega a amostra do índice atual
        nlms_type_t u_val = filter->delay_line[idx];

        // Calcula a saida estimada, com o peso e a entrada atual
        y += filter->weights[i] * u_val;
        // Energia do dado atual
        norm_sq += u_val * u_val;
    }

    // Erro da predição dentre o estimado e real
    nlms_type_t error = desired_sample - y;
    
    // Agora vem o filtro NLMS em sí:
    // Calcula o fator de aprendizado
    nlms_type_t update_factor = (filter->mu * error) / (norm_sq + filter->delta);

    // Atualização dos pesos do filtro
    for (size_t i = 0; i < filter->N; i++)
    {
        size_t idx = (filter->head_index + filter->N - i) % filter->N;
        filter->weights[i] += update_factor * filter->delay_line[idx];
    }

    // atualiza as variáveis de estado
    filter->last_error = error;
    filter->last_output = y;

    filter->head_index = (filter->head_index + 1) % filter->N;

    return y;
}

// Reseta todos os pesos e o Buffer
void NLMSFilter_Reset(NLMSFilter *filter, nlms_type_t default_weight_value, nlms_type_t default_buffer_value)
{
    for (size_t i = 0; i < filter->N; ++i) {
        filter->weights[i] = default_weight_value;
        filter->delay_line[i] = default_buffer_value;
    }
    filter->head_index = 0;
    filter->last_error = 0.0f;
    filter->last_output = 0.0f;
}

void NLMSFilter_SetLearningRate(NLMSFilter *filter, nlms_type_t new_mu) {
    filter->mu = new_mu;
}

void NLMSFilter_SetRegularization(NLMSFilter *filter, nlms_type_t new_delta) {
    filter->delta = new_delta;
}

nlms_type_t NLMSFilter_GetLastError(const NLMSFilter *filter) {
    return filter->last_error;
}

////////////////////// Ultima saida do filtro ///////////////////////////
nlms_type_t NLMSFilter_GetLastOutput(const NLMSFilter *filter) {
    return filter->last_output;
}

///////////////// Retorna a matriz de pesos do filtro ////////////////////
void NLMSFilter_GetOrderedBuffer(const NLMSFilter *filter, nlms_type_t *ordered_out)
{
    for (size_t i = 0; i < filter->N; ++i) {
        size_t idx = (filter->head_index + i) % filter->N;
        ordered_out[i] = filter->delay_line[idx];
    }
}