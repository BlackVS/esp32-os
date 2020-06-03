#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"

#include <cstring>

#include "esp32_spi.h"

#define USE_POLLING_TRANSMIT 1

///////////////////////////////////////////////////////////////////////////////
#undef TAG
#define TAG "SPIBUS"
ESP32_SPIbus::ESP32_SPIbus( spi_host_device_t host, gpio_num_t  pin_mosi, gpio_num_t pin_miso, gpio_num_t pin_sclk, int max_transfer_sz)
     :  m_host(host),
        m_pin_mosi(pin_mosi),
        m_pin_miso(pin_miso),
        m_pin_sclk(pin_sclk),
        m_max_transfer_sz(max_transfer_sz)

{
    //ESP_LOGD(TAG, "%s()", __FUNCTION__);
}

esp_err_t ESP32_SPIbus::begin() 
{
    //ESP_LOGD(TAG, "%s()", __FUNCTION__);
    esp_err_t ret=ESP_OK;

    if(m_inited)
    {
       ESP_LOGW(TAG, "Bus already initialized, busId=%d", m_host); 
       return ESP_OK;//normal if few devices on same bus
    }

    spi_bus_config_t config = {};
    config.mosi_io_num = m_pin_mosi;
    config.miso_io_num = m_pin_miso;
    config.sclk_io_num = m_pin_sclk;
    config.quadwp_io_num = -1;  // -1 not used
    config.quadhd_io_num = -1;  // -1 not used
    config.max_transfer_sz = m_max_transfer_sz;

    //ESP_LOGD(TAG, "BUSCFG: bus=%s",  m_host==VSPI_HOST ? "VSPI_HOST" : ( m_host==HSPI_HOST? "HSPI_HOST" : "SPI_HOST" )); 
    ESP_LOGD(TAG, "BUSCFG: bus=%s",  get_host_str()); 
    ESP_LOGD(TAG, "BUSCFG: miso=%d", config.miso_io_num);
    ESP_LOGD(TAG, "BUSCFG: mosi=%d", config.mosi_io_num);
    ESP_LOGD(TAG, "BUSCFG: sclk=%d", config.sclk_io_num); 
    ESP_LOGD(TAG, "BUSCFG: max_transfer_sz=%d", config.max_transfer_sz); 

    ret=spi_bus_initialize(m_host, &config, 1);  // 0 DMA not used
    ESP_ERROR_CHECK(ret);
    // THIS IS HACK TO GET NOTIFICATIONS ON DC PIN CHANGE
    //ssd1306_registerPinEvent(m_dc, OnDcChange, this);   
    m_inited=true;     
    return ret;

}

esp_err_t ESP32_SPIbus::end() 
{
    esp_err_t ret=ESP_OK;
    if(!m_inited)
    {
       ESP_LOGW(TAG, "Bus already freed, busId=%d", m_host); 
       return ESP_OK;//normal if few devices on same bus
    }
    ret=spi_bus_free(m_host);
    ESP_ERROR_CHECK(ret);
    m_inited=false;
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////
#undef TAG
#define TAG "SPIDEV"

esp_err_t ESP32_SPIdevice::begin()
{
    m_bus.begin();

    //ESP_LOGD(TAG, "%s()", __FUNCTION__);
    esp_err_t ret=ESP_OK;

    ESP_LOGD(TAG,"SPI dev initialize:");
    ESP_LOGD(TAG," host=%s", m_bus.get_host_str() );
    ESP_LOGD(TAG," CS  pin = %d",m_pin_cs);
    ESP_LOGD(TAG," DC  pin = %d",m_pin_dc);
    ESP_LOGD(TAG," RST pin = %d",m_pin_reset);
    ESP_LOGD(TAG," freq    = %d",m_freq);

    gpio_set_direction( m_pin_cs, GPIO_MODE_OUTPUT );
	gpio_set_level( m_pin_cs, 0 );

	gpio_set_direction( m_pin_dc, GPIO_MODE_OUTPUT );
	gpio_set_level( m_pin_dc, 0 );

    reset();

    if(m_inited)
    {
       ESP_LOGW(TAG, "Device already initialized"); 
       return ESP_OK;//normal if few devices on same bus
    }

    spi_device_interface_config_t devcfg {};
    devcfg.clock_speed_hz = m_freq;
    devcfg.spics_io_num = m_pin_cs;
    devcfg.queue_size = 7;
    devcfg.flags = SPI_DEVICE_NO_DUMMY;
    ret=spi_bus_add_device( m_bus.get_host_id(), &devcfg, &m_dev_handle);
    ESP_ERROR_CHECK(ret);
    m_inited=true;
    return ret;
}

esp_err_t ESP32_SPIdevice::end()
{
    //ESP_LOGD(TAG, "%s()", __FUNCTION__);
    esp_err_t ret=ESP_OK;
    if(!m_inited)
    {
       ESP_LOGW(TAG, "Device already released"); 
       return ESP_OK;//normal if few devices on same bus
    }
    if(m_dev_handle) {
        ret=spi_bus_remove_device(m_dev_handle);
        m_dev_handle=NULL;
    }
    ESP_ERROR_CHECK(ret);
    m_inited=false;
    return ret;
}

esp_err_t ESP32_SPIdevice::reset(uint delayMs)
{
    //ESP_LOGD(TAG, "%s()", __FUNCTION__);
    gpio_set_direction( m_pin_reset, GPIO_MODE_OUTPUT );
    gpio_set_level(m_pin_reset, 1);

    /* Wait at least 10ms after VCC is up for LCD */
    spi_delay(10);
    
    /* Perform reset operation of LCD display */
    gpio_set_level(m_pin_reset, 0);
    spi_delay(10);

    gpio_set_level(m_pin_reset, 1);
    spi_delay(delayMs);

    return ESP_OK;
}

esp_err_t ESP32_SPIdevice::write_byte(uint8_t data, bool bCaching)
{
	esp_err_t ret=ESP_OK;
    spi_transaction_t trans = {};
    if(!m_dev_handle) {
        return ESP_ERR_INVALID_ARG;
    }
    if(!bCaching)
    {
        assert(m_data_size==0);
        trans.length = sizeof(data) * 8; //in bits
        //trans.tx_buffer = &data; //tx_data preferred here!!!
        trans.tx_data[0]=data;
        trans.flags=SPI_TRANS_USE_TXDATA;
        #ifdef USE_POLLING_TRANSMIT
        ret = spi_device_polling_transmit( m_dev_handle, &trans);
        #else
        ret = spi_device_transmit( m_dev_handle, &trans);
        #endif
        ESP_ERROR_CHECK(ret); 
    } else {
        if(m_data_size==sizeof(m_data_buf)) 
            flush_data();
        assert(m_data_size<sizeof(m_data_buf));
        m_data_buf[m_data_size++]=data;
        if(m_data_size==sizeof(m_data_buf)) 
            flush_data();
    }
	
    return ret;
}

esp_err_t ESP32_SPIdevice::flush_data()
{
    esp_err_t ret = ESP_OK;
    if(m_data_size) {
        ret=write_bytes(m_data_buf, m_data_size);
        m_data_size=0;
    }
    ESP_ERROR_CHECK(ret);
    return ret;
}

esp_err_t ESP32_SPIdevice::write_bytes(const uint8_t* buf, uint buflen)
{
	esp_err_t ret=ESP_OK;
    spi_transaction_t trans = {};
    if(!m_dev_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    if(buflen<=4) {
        memcpy( &trans.tx_data[0], buf, buflen);
        trans.flags=SPI_TRANS_USE_TXDATA;
    } else {
    	trans.tx_buffer = buf; 
    }
	trans.length = buflen * 8; //in bits
    #ifdef USE_POLLING_TRANSMIT
    ret = spi_device_polling_transmit( m_dev_handle, &trans);
    #else
	ret = spi_device_transmit( m_dev_handle, &trans);
    #endif
	ESP_ERROR_CHECK(ret); 
    return ret;
}

esp_err_t ESP32_SPIdevice::set_data_mode()
{
    //SPI_LOG("\nD: ");
	esp_err_t ret=ESP_OK;
    if ( m_pin_dc >= 0 )
        ret= gpio_set_level( m_pin_dc, 1);
	ESP_ERROR_CHECK(ret); 
    return ESP_OK;
}

esp_err_t ESP32_SPIdevice::set_cmd_mode()
{
    //SPI_LOG("\nD: ");
	esp_err_t ret=ESP_OK;
    if ( m_pin_dc >= 0 )
        ret= gpio_set_level( m_pin_dc, 0); // else send 0x00?
	ESP_ERROR_CHECK(ret); 
    return ESP_OK;
}

esp_err_t ESP32_SPIdevice::write_cmd(uint8_t cmd)
{
    set_cmd_mode();
    return write_byte(cmd);
}

esp_err_t ESP32_SPIdevice::write_16bits(uint16_t word)
{
    esp_err_t ret=ESP_OK;
    if(!m_dev_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    //uint8_t buf[2];
	//buf[0] = (word >> 8) & 0xFF;
	//buf[1] = word & 0xFF;


    spi_transaction_t trans = {};
    trans.tx_data[0]=(word >> 8) & 0xFF;
    trans.tx_data[1]=word & 0xFF;
    trans.flags=SPI_TRANS_USE_TXDATA;

	trans.length = 2 * 8; //in bits

	//trans.tx_buffer = &buf;         //tx_data preferred here!!!
    #ifdef USE_POLLING_TRANSMIT
    ret = spi_device_polling_transmit( m_dev_handle, &trans);
    #else
	ret = spi_device_transmit( m_dev_handle, &trans);
    #endif
	ESP_ERROR_CHECK(ret); 
    return ret;
}


esp_err_t ESP32_SPIdevice::write_2x16bits(uint16_t word1, uint16_t word2)
{
    esp_err_t ret=ESP_OK;

    if(!m_dev_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    //uint8_t buf[2];
	//buf[0] = (word >> 8) & 0xFF;
	//buf[1] = word & 0xFF;


    spi_transaction_t trans = {};
    trans.tx_data[0]=(word1 >> 8) & 0xFF;
    trans.tx_data[1]=word1 & 0xFF;
    trans.tx_data[2]=(word2 >> 8) & 0xFF;
    trans.tx_data[3]=word2 & 0xFF;
    trans.flags=SPI_TRANS_USE_TXDATA;

	trans.length = 4 * 8; //in bits

	//trans.tx_buffer = &buf;         //tx_data preferred here!!!
    #ifdef USE_POLLING_TRANSMIT
    ret = spi_device_polling_transmit( m_dev_handle, &trans);
    #else
	ret = spi_device_transmit( m_dev_handle, &trans);
    #endif
	ESP_ERROR_CHECK(ret); 
    return ret;
}
