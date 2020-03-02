#include "math.h" 





//float val[] = {0.5, 0.10, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1, 1.10 ,1.20, 1.30, 1.40, 1.50}; 
float val[128]; 

volatile float test = 0; 
unsigned long my_time; 
unsigned long Henrik_time; 
unsigned long standard_time; 


float relativeDifference(float table, float test){
    float tmp; 
    tmp = ((test - table)/table)*100;
    return tmp; 
  }
  
void setup() {
  Serial.begin(9600);
 //Test array is created
 int size = sizeof(val)/sizeof(float); 
  for(int i = 0; i <size; i++ ){
    val[i] = 1*i; 
  }
  my_time = micros(); 
  for (int i = 0; i < size ; i ++ ){
  test = my_sin3(val[i]); 
  }
  my_time = micros() - my_time;
  Serial.print("mytime: "); 
  Serial.println(my_time); 

  standard_time = micros(); 
  for (int i = 0; i < size ; i ++ ){
    test = sin(val[i]); 
  }
  standard_time = micros() - standard_time; 
  Serial.print("standardTime: "); 
  Serial.println(standard_time); 

//The inputs and the outpus of the different sines is printed
for (int i = 0; i < size ; i ++ ){
   Serial.print("input : "); 
   Serial.print(val[i]);
   Serial.print(" StandardSineResult: ");
   Serial.print (sin(val[i]), 10); 
   Serial.print (" my_sinResult:  "); 
   Serial.print (my_sin3(val[i]), 10); 
   Serial.print (" my_sinRelative:  ");
   Serial.println(relativeDifference(sin(val[i]), my_sin3(val[i])), 10); 
  }



}

void loop() {

}

//Sine that works for mosts inputs not tested thoroughly
//Not faster than Henriks implementation
float my_sin(float val){
  //All this is mapping the float into the correct part of the sine function so that even though the
  //taylor polynominal only maps 0 to pi/2 the correct output is given. 
   long fval; 
   const long point = 32768; 
   const float Div2pi = 0.159154943091895;
   const float pi2 = 6.283185307179586;
   const float pi = 3.141592653589793;
   
   const long fDiv2pi = Div2pi * point; 
   const long fpi2 = pi2*point; 
   const long fpi = pi * point; 

   const long a1 = 0.166666666666667*point; 
   const long a2 = 0.008333333333333*point;
   const float ppoint = 0.000030518;

    int8_t sign = 0;
    float n = 0;
    if (val < 0) {
        sign = 1;
        val = -val; 
    }
    else sign = 0;
    
    n  = (long)(val * Div2pi);
    fval = (long)((val - n*pi2)*point);    
    
if (fval <= (long) (3.141592653589793*point)){
    if (fval > (long)(1.570796326794897*point)){
        fval = fpi - fval ;
    }
    else{
        fval = fval;
    }
}
else {
    if( fval <= (long)(4.712388980384690*point)){
        fval = -(fval - fpi);
    }
    else{
        fval = -(fpi2 - fval);
      }
    }
    fval = taylorp15(fval);
    if (sign == 1){
        return - (fval * ppoint) ;
    }
    else {
        return (fval*ppoint);
    }
  return -1; 
}

//Taylorpoly using fixedpoint and horners method
long taylorp15(long val){
   const  long point = 32768; //this corresponds to point 15 
   const  long a1 = 0.166666666666667*point; 
   const  long a2 = 0.008333333333333*point;
   long res = 0; 
  res = ((val * a2) >> 16) << 1; //this trick sacrifces a bit of precision for more than three times the speed 
  res = ((res * val) >> 16) <<1; //of just shifting 15 in one go
  res = res - a1; 
  res = ((res * val) >> 16) <<1 ; 
  res = ((res * val) >> 16) <<1 ; 
  res = ((res * val) >> 16) <<1; 
  res += val; 
  return res; 
}

//implementation that only works up to pi/2
//This one is slightly slower than Henriks implementation.
//It can howewer be speed up significantly if you don't mind using fixedpoint all the way
//Since the 2 float multiplications are timeconsuming.
//Im not entirely sure its actually below 2% reletive deviation
//The fifth order taylorpolynominal is in theory but the implementation might cause issues.    
float my_sin2(float val){
  const float ppoint = 0.000030518000;
  const long point = 32768; //corresponds to point 15
  const  long a1 = 0.166666666666667*point; 
  const  long a2 = 0.008333333333333*point;
  long tmp = (long)(val*point); 
  long res = 0; 
  res = ((tmp * a2) >> 16) << 1 ; //this trick sacrifces a bit of precision for more than three times the speed for the shift
  res = ((res * tmp) >> 16) <<1 ; //of just shifting 15 in one go
  res = res - a1; 
  res = ((res * tmp) >> 16) <<1 ; 
  res = ((res * tmp) >> 16) <<1 ; 
  res = ((res * tmp) >> 16) <<1 ; 
  res += tmp; 
  return (float)(res*ppoint); 
}

//implementation that only works up to pi/2
//This one takes 35% of the time of the standard implementation in mixed case input 
//with refference in the standard sin() the relative difference is less than 2%    
float my_sin3(float val){
  long res = 0; 
  long tmp; 
  const float ppoint = 0.000030518000;
  const long point = 32768; //corresponds to point 15
  const long a01 = 0.011*point;
  const long a11 = 1*point;
  const long a21 = -0.15*point; 

  const long a02 = -0.08*point; 
  const long a12 = 1.32*point;
  const long a22 = -0.4*point; 
  if(val < 0.34){
    return val; 
  }
  if(val < 0.85){
  tmp = (long)(val*point);
  res = ((tmp*a21) >> 16) <<1 ;
  res = res + a11; 
  res = ((res*tmp) >> 16) <<1; 
  res = res + a01;   
  return res*ppoint; 
  }
  
  tmp =(long)(val*point);
  res = ((tmp*a22) >> 16) <<1 ;
  res = res + a12; 
  res = ((res*tmp) >> 16) <<1; 
  res = res + a02;   
  return res*ppoint; 
}
  
