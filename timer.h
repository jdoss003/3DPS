/*
 * File: timer.h
 * Author : Justin Doss
 *
 * Methods to control timer 0/3 which can be configured for ms time periods.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

void TimerOnA();
void TimerOffA();
void TimerSetA(unsigned long M);

void TimerOnB();
void TimerOffB();
void TimerSetB(unsigned long M);

#ifdef __cplusplus
}
#endif

#endif //TIMER_H
