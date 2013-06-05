#define VISITOR_MAX 4
#define CAR_MAX 3
#define LIMO_MAX 4
#define VALET_MAX 5

Lock* carLock[CAR_MAX];
Lock* limoLock[LIMO_MAX];
Condition* carCV[CAR_MAX];
Condition* limoCV[LIMO_MAX];