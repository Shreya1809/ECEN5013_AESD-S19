#include "includes.h"
#include "light.h"
#include "main.h"
#include "logger.h"
#include "bbgled.h"
#include "mysignal.h"
#include "mytimer.h"
#include "lightSensor.h"
#include "heartbeat.h"
#include "bist.h"
/* mraa header */
#include "mraa/gpio.h"

/* gpio declaration */
#define GPIO_PIN_19     19
#define APDS_INT_PIN    GPIO_PIN_19

static volatile float lux_val = 0.0;
static sig_atomic_t stop_thread_light = 0;
int l_count = 0;
pthread_mutex_t light_var_lock = PTHREAD_MUTEX_INITIALIZER;


static inline void setLuxVar(float lux)
{
    pthread_mutex_lock(&light_var_lock);
    lux_val = lux;
    pthread_mutex_unlock(&light_var_lock);
} 

static inline float getLuxVar()
{
    float lux = 0;
    pthread_mutex_lock(&light_var_lock);
    lux = lux_val;
    pthread_mutex_unlock(&light_var_lock);
    return lux;
} 

static int readAndUpdateLight(void)
{
    float lux = 0.0;
    if(l_count > 4)
    {
        stop_thread_light = 1;   
    }
    int ret = APDS9301_getlight(&lux);
    if(ret)
    {
        LOG_ERROR(TEMP_TASK, "APDS9301 light Sensor Disconnected: %d", ret);
        l_count++;
        return ret;
    } 
    LOG_INFO(LIGHT_TASK,"The light is %f",lux);
    setLuxVar(lux); 
    return 0;
}

mraa_gpio_context APDS9301_IntPinSetup(int mraaPinNumber)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_gpio_context lightSensorIntPin;
    mraa_init();
    lightSensorIntPin = mraa_gpio_init(mraaPinNumber);
    if (lightSensorIntPin == NULL) {
        LOG_ERROR(LIGHT_TASK, "Failed to initialize GPIO %d", mraaPinNumber);
        return NULL;
    }
    status = mraa_gpio_dir(lightSensorIntPin, MRAA_GPIO_IN);
    if (status != MRAA_SUCCESS) {
        LOG_ERROR(LIGHT_TASK, "Failed to set input direction GPIO %d", mraaPinNumber);
        mraa_gpio_close(lightSensorIntPin);
        return NULL;
    }
    return lightSensorIntPin;
}

int APDS9301_checkINTPIN(mraa_gpio_context lightSensorIntPin)
{
    int status = mraa_gpio_read(lightSensorIntPin);
    if (status == -1) 
    {
        LOG_ERROR(LIGHT_TASK, "Failed to read GPIO %d", mraa_gpio_get_pin(lightSensorIntPin));
    }
    LOG_DEBUG(LIGHT_TASK, "APDS INT Pin value:%d",status);
    return status;
}

void kill_light_thread(void)
{
    printf("light exit\n");
    stop_thread_light = 1;
    stop_thread_light++;    
}

float getLight(void)
{
    float lux = getLuxVar();
    return lux;
}

static void giveSemSensor(union sigval no)
{
    sem_post(&light_sem);
}

void *light_task(void *threadp)
{
    uint8_t th_lowlow,th_hilow,th_lowhi,th_hihi;
    sem_init(&light_thread_sem,0,0);
    sem_wait(&light_thread_sem);
    if(!CheckBistResult())
    {
        goto exit;
    }
    LOG_INFO(LIGHT_TASK,"Light task thread spawned");
    mraa_gpio_context interrupt_gpio_pin =  APDS9301_IntPinSetup(APDS_INT_PIN);
    if(interrupt_gpio_pin == NULL)
    {
        goto exit;
    }
    LOG_INFO(LIGHT_TASK, "APDS Int pin setup done");
    int ret1 = APDS9301_writeTHRESH_lowlow(0x0); //write 2 byte
    int ret4 = APDS9301_writeTHRESH_highlow(1200);//write 2 byte
    int ret2 = APDS9301_readTHRESH_lowlow(&th_lowlow);
    ret2 = APDS9301_readTHRESH_lowhigh(&th_lowhi);
    ret2 = APDS9301_readTHRESH_highhigh(&th_hihi);
    ret2 = APDS9301_readTHRESH_highlow(&th_hilow);
    LOG_DEBUG(LIGHT_TASK,"read threshold low val is 0x%02x",th_lowlow | ((uint16_t)th_lowhi)<<8);
    LOG_DEBUG(LIGHT_TASK,"read threshold high val is 0x%02x",th_hilow | ((uint16_t)th_hihi)<<8);
    APDS9301_interruptCTRLreg(ENABLE);

    timer_t light_timer;
    sem_init(&light_sem,0,0);
    if(maketimer(&light_timer, &giveSemSensor) != 0)
    {
        perror("MakeTimer fail");
    }
    startTimer(light_timer);
    while(!stop_thread_light)
    {
        set_heartbeatFlag(LIGHT_TASK);
        if(sem_wait(&light_sem) == 0)
        {
            readAndUpdateLight();
            int pinValue = APDS9301_checkINTPIN(interrupt_gpio_pin);
            if(pinValue == 0)
            {
                LOG_INFO(LIGHT_TASK, "Light sensor Threshold exceeded");
                APDS9301_intClear();
            }
        }
    }
exit:
    PRINTLOGCONSOLE("Light task thread exiting");
    LOG_INFO(LIGHT_TASK,"Light task thread exiting");
    return NULL;
}