#include "bsp_usart.h"
#include "string.h"
/**
 * @brief ��Ҫ����dma,�����ж�
 *
 */
/* usart service instance, modules' info would be recoreded here using USARTRegister() */
/* usart����ʵ��,����ע����usart��ģ����Ϣ�ᱻ���������� */
static uint8_t idx;
static USARTInstance *usart_instance[DEVICE_USART_CNT] = {NULL};

/**
 * @brief �������ڷ���,����ÿ��ʵ��ע��֮���Զ����ý���,��ǰʵ��ΪDMA����,�����������IT��BLOCKING����
 *
 * @todo ���ڷ������ÿ��ʵ��ע��֮���Զ����ý���,��ǰʵ��ΪDMA����,�����������IT��BLOCKING����
 *       ���ܻ�Ҫ���˺����޸�Ϊextern,ʹ��module���Կ��ƴ��ڵ���ͣ
 *
 * @param _instance instance owned by module,ģ��ӵ�еĴ���ʵ��
 */
void USARTServiceInit(USARTInstance *_instance)
{
    HAL_UARTEx_ReceiveToIdle_DMA(_instance->usart_handle, _instance->recv_buff, _instance->recv_buff_size);
    // �ر�dma half transfer�жϷ�ֹ���ν���HAL_UARTEx_RxEventCallback()
    // ����HAL���һ�����ʧ��,����DMA�������/������Լ�����IDLE�ж϶��ᴥ��HAL_UARTEx_RxEventCallback()
    // ����ֻϣ�������һ�ֺ͵��������,���ֱ�ӹر�DMA�봫���ж�
    __HAL_DMA_DISABLE_IT(_instance->usart_handle->hdmarx, DMA_IT_HT);
}

void USARTRegister(USARTInstance *instance, USART_Init_Config_s *init_config)
{
    memset(instance, 0, sizeof(USARTInstance));

    instance->usart_handle = init_config->usart_handle;
    instance->recv_buff_size = init_config->recv_buff_size;
    instance->module_callback = init_config->module_callback;
    instance->param = init_config->param;
    usart_instance[idx++] = instance;
    USARTServiceInit(instance);
}

/* @todo ��ǰ����������ʽ�ϵķ�װ,����Ҫ��һ�������Ƿ�module����Ϊ��bsp��ȫ���� */
void USARTSend(USARTInstance *_instance, uint8_t *send_buf, uint16_t send_size, USART_TRANSFER_MODE mode)
{
    switch (mode)
    {
    case USART_TRANSFER_BLOCKING:
        HAL_UART_Transmit(_instance->usart_handle, send_buf, send_size, 100);
        break;
    case USART_TRANSFER_IT:
        HAL_UART_Transmit_IT(_instance->usart_handle, send_buf, send_size);
        break;
    case USART_TRANSFER_DMA:
        HAL_UART_Transmit_DMA(_instance->usart_handle, send_buf, send_size);
        break;
    default:
        while (1)
            ; // illegal mode! check your code context! ��鶨��instance�Ĵ���������,���ܳ���ָ��Խ��
        break;
    }
}

/* ���ڷ���ʱ,gstate�ᱻ��ΪBUSY_TX */
uint8_t USARTIsReady(USARTInstance *_instance)
{
    if (_instance->usart_handle->gState | HAL_UART_STATE_BUSY_TX)
        return 0;
    else
        return 1;
}

/**
 * @brief ÿ��dma/idle�жϷ���ʱ��������ô˺���.����ÿ��uartʵ������ö�Ӧ�Ļص����н�һ���Ĵ���
 *        ����:�Ӿ�Э�����/ң��������/����ϵͳ����
 *
 * @note  ͨ��__HAL_DMA_DISABLE_IT(huart->hdmarx,DMA_IT_HT)�ر�dma half transfer�жϷ�ֹ���ν���HAL_UARTEx_RxEventCallback()
 *        ����HAL���һ�����ʧ��,����DMA�������/������Լ�����IDLE�ж϶��ᴥ��HAL_UARTEx_RxEventCallback()
 *        ����ֻϣ���������ֱ�ӹر�DMA�봫���жϵ�һ�ֺ͵�������� @todo ����ʹ��DMA����жϣ��Ͳ���lagacy��HAL_UARTEx_RxEventCallback()��
 *
 * @param huart �����жϵĴ���
 * @param Size �˴ν��յ�����������,��ʱû��
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    for (uint8_t i = 0; i < idx; ++i)
    { // find the instance which is being handled
        if (huart == usart_instance[i]->usart_handle)
        { // call the callback function if it is not NULL
            if (usart_instance[i]->module_callback != NULL)
            {
                usart_instance[i]->module_callback(usart_instance[i]->param); // �������ݵĻص�����,������module��ָ�����������
                memset(usart_instance[i]->recv_buff, 0, Size);                // ���ս��������buffer,���ڱ䳤�����Ǳ�Ҫ��
            }
            HAL_UARTEx_ReceiveToIdle_DMA(usart_instance[i]->usart_handle, usart_instance[i]->recv_buff, usart_instance[i]->recv_buff_size);
            __HAL_DMA_DISABLE_IT(usart_instance[i]->usart_handle->hdmarx, DMA_IT_HT);
            return; // break the loop
        }
    }
}

/**
 * @brief �����ڷ���/���ճ��ִ���ʱ,����ô˺���,��ʱ�������Ҫ���ľ���������������
 *
 * @note  ����Ĵ���:��żУ��/���/֡����
 *
 * @param huart ��������Ĵ���
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    for (uint8_t i = 0; i < idx; ++i)
    {
        if (huart == usart_instance[i]->usart_handle)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(usart_instance[i]->usart_handle, usart_instance[i]->recv_buff, usart_instance[i]->recv_buff_size);
            __HAL_DMA_DISABLE_IT(usart_instance[i]->usart_handle->hdmarx, DMA_IT_HT);

            // LOGWARNING("[bsp_usart] USART error callback triggered, instance idx [%d]", i);
            return;
        }
    }
}