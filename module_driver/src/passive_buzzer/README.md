sys_config.fex:

[pwm6]
pwm_used        = 1
pwm_positive    = port:PA26<4><0><2><default>


menuconfig:
Drivers Options  --->
    soc related device drivers  --->
        PWM devices --->
            [*] enable pwm driver
            [ ]   enable pwm hal APIs test command