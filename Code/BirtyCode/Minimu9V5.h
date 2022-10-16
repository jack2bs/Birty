/*
 * Minimu9V5.h
 *
 * Created: 6/8/2022 9:17:32 PM
 *  Author: Jack2bs
 */ 


#ifndef MINIMU9V5_H_
#define MINIMU9V5_H_

void addReadIMUwhoAmI(int index);
void addReadGyroZInstr(int index);

void GYRORawToString(int raw, char* str);

void IMUinit(int index);
void magInit(int index);
void minimu9V5Init(int index);
void minimu9V5Task(int index);

void setMinimu9V5TaskFlag();
void clearMinimu9V5TaskFlag();
int getMinimu9V5TaskFlag();

double updateHeading(int index, int dt);
double getHeading();
int getHeadingInt();
void resetHeading();


#endif /* MINIMU9V5_H_ */