#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

void five_s_timer_handler(struct k_timer *timer);
void three_s_timer_handler(struct k_timer *timer);


K_TIMER_DEFINE(five_s_timer, five_s_timer_handler, NULL);
K_TIMER_DEFINE(three_s_timer, three_s_timer_handler, NULL);


void five_s_timer_handler(struct k_timer *timer)
{
    printk("It has been five seconds.\n");
}

void three_s_timer_handler(struct k_timer *timer)
{
    printk("It has been three seconds.\n");
}

void main(void)
{
	k_timer_start(&five_s_timer, K_SECONDS(5), K_SECONDS(5));
	k_timer_start(&three_s_timer, K_SECONDS(3), K_SECONDS(3));
	printk("Started timers. Please wait.\n");
}
