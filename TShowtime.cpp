#include "TShowtime.h"

TShowTime::TShowTime()
{
     daySecond = 0;  //86400
     day = " day ";
     hourSecond = 0;  //3600
     hour = " hour ";
     minutSecond = 0; //60
     minut = " minut ";
     Second = " sec ";
     temp = 0;

}

 QString TShowTime::getTimestr(time_t lostT)
 {
     time_t lostClock = lostT;


     //lostClock = 3700;

     day = " day ";
     hour = " hour ";
     minut = " minut ";
     temp = 0;


             if (lostClock > 86400)
             {
                 daySecond = lostClock / 86400;
                 hourSecond =  lostClock % 86400;

                 if (hourSecond > 3600)
                 {
                    temp = hourSecond;
                    hourSecond = temp / 3600;
                    minutSecond = temp % 3600;

                    if (minutSecond > 60)
                    {
                        temp = minutSecond;
                        minutSecond = temp / 60;
                        second = temp % 60;
                    }

                    else
                    {
                        second = minutSecond;
                        minutSecond = 0;

                    }
                 }

                 else
                 {

                     if (hourSecond > 60)
                     {
                        minutSecond = hourSecond / 60;
                        second = hourSecond % 60;
                        hourSecond = 0;
                     }
                     else
                     {
                       second = hourSecond;
                       minutSecond = 0;
                       hourSecond = 0;

                     }


                 }
             }


             ////////////////////////////////////

             else if (lostClock > 3600)
             {
                    daySecond = 0;
                    hourSecond = lostClock / 3600;
                    minutSecond = lostClock % 3600;

                    if (minutSecond > 60)
                    {
                        temp = minutSecond;
                        minutSecond = temp / 60;
                        second = temp % 60;
                    }

                    else
                    {
                        second = minutSecond;
                        minutSecond = 0;


                    }

             }

             else if (lostClock > 60)
             {

                  daySecond = 0;
                  hourSecond = 0;
                  minutSecond = lostClock / 60;
                  second = lostClock % 60;

             }

             else
             {
                 daySecond = 0;
                 hourSecond = 0;
                 minutSecond = 0;
                 second = lostClock;
             }


             sprintf(daySecondChar, "%ld", daySecond);
             sprintf(hourSecondChar, "%ld", hourSecond);
             sprintf(minutSecondChar, "%ld", minutSecond);
             sprintf(secondChar, "%ld", second);


             daySecondQ = (QString)daySecondChar;
             hourSecondQ = (QString)hourSecondChar;
             minutSecondQ = (QString)minutSecondChar;
             secondQ = (QString)secondChar;


             if (daySecond == 0)
             {
                 daySecondQ = "";
                 day = "";

             }

             if (hourSecond == 0)
             {
                 hourSecondQ = "";
                 hour = "";

             }

             if (minutSecond == 0)
             {
                 minutSecondQ = "";
                 minut = "";

             }

            strLostTime = daySecondQ +  day + hourSecondQ + hour + minutSecondQ + minut + secondQ + Second;

            return strLostTime;



 }

