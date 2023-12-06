sys_config.fex:

[pwm0]
pwm_used        = 1
pwm_positive    = port:PA8<4><0><2><default>


menuconfig:
Drivers Options  --->
    soc related device drivers  --->
        PWM devices --->
            [*] enable pwm driver
            [ ]   enable pwm hal APIs test command