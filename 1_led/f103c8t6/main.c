

typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;


struct rcc {
  volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, 
                    APB2ENR, APB1ENR, BDCR, CSR;
};
#define RCC ((struct rcc *) 0x40021000)

struct gpio {
  volatile uint32_t CRL, CRH, IDR, ODR, BSRR, BRR, LCKR;
};
#define GPIOC ((struct gpio *) (0x40011000 ))


static inline void gpio_set_mode(void) {

    GPIOC->CRH &= ~(0x0fU << 20);         // Clear existing setting

    GPIOC->CRH |= 0x20 << 16;       // Set new mode 
}

static inline void gpio_write(uint8_t val) {

    if (val == 1)
        GPIOC->BSRR = 0x20 << 8;
    else
        GPIOC->BSRR = 0x20 << 24;
}

static inline void spin(volatile uint32_t count) {
  while (count--) asm("nop");
}

int main(void) {

    struct rcc* rcc = (void *)0x40021000;

    struct gpioc* gpioc = (void *)0x40011000;

    rcc->APB2ENR |= (1<<4);

    //RCC->APB2ENR |= (1<<4);

    gpio_set_mode();


  for (;;) {
    gpio_write(1);
    spin(999999);
    gpio_write(0);
    spin(999999);
  }
  return 0;
}


__attribute__((naked, noreturn)) void _reset(void) {
  // memset .bss to zero, and copy .data section to RAM region
  extern long _sbss, _ebss, _sdata, _edata, _sidata;
  for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
  for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

  main();             // Call main()
  for (;;) (void) 0;  // Infinite loop in the case if main() returns
}

extern void _estack(void);  // Defined in link.ld

// 16 standard and 91 STM32-specific handlers
__attribute__((section(".vectors"))) void (*const tab[16 + 60])(void) = {
    _estack, _reset};


