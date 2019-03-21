#ifndef TIMER_H_
#define TIMER_H_
/**
 * @brief initialisation of timer
 * 
 * @param timerID 
 * @return int 
 */
int maketimer(timer_t *timerID);
/**
 * @brief starting the timer
 * 
 * @param timerID 
 * @return int 
 */
void startTimer(timer_t timerID);
/**
 * @brief 
 * 
 * @param sigval 
 */
static void giveSemSensor(union sigval no)
{
    sem_post(&temp_sem);
    sem_post(&light_sem);
    //fprintf(fp,"[%lf] CPU UTILISATION -> " ,getTimeMsec());
}
#endif
