/*
 * dist_task.h
 *
 *  Created on: Apr 13, 2019
 *      Author: Shreya
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

void Dist_init(void);
int32_t get_Dist(void);
void Timer0IntHandler(void);
void PortEIntHandler(void);
void Dist_alarm(int option);


#endif /* ULTRASONIC_H_ */
