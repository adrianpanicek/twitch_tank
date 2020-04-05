#define MOTOR_PWM_RESOLUTION 8
#define MOTOR_PWM_FREQ 1000

#define MOTOR_CHAN_LEFT_FRONT 0
#define MOTOR_PIN_LEFT_FRONT 17

#define MOTOR_CHAN_LEFT_BACK 1
#define MOTOR_PIN_LEFT_BACK 16

#define MOTOR_CHAN_RIGHT_FRONT 2
#define MOTOR_PIN_RIGHT_FRONT 4

#define MOTOR_CHAN_RIGHT_BACK 3
#define MOTOR_PIN_RIGHT_BACK 2

struct Movement {
    double front = 0;
    double side = 0;
};

typedef void (*motor_callback)(Movement actualMovement);

void motor_setup(motor_callback callback);
void motor_loop();
void motor_add_movement(Movement movement);