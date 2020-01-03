#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <wiringpi/wiringPi.h>

#include "gpio_mutex.h"

static pid_t pid;

// mutexes
static shared_mutex_t *i2c0Mutex = NULL;
static shared_mutex_t *spi0Mutex = NULL;

// mcp23017 (on i2c0)
static int mcp23017_node_searched = 0;
static struct wiringPiNodeStruct *mcp23017_node = NULL;
static int mcp23017_pin_base = 100;
static size_t mcp23017_data_len = sizeof(unsigned int /*data2*/) + sizeof(unsigned int /*data3*/);

// flags inidicating if a mutex is being used currently
static int usingI2C0Mutex = 0;
static int usingSPI0Mutex = 0;

void i2c0_mutex_init() {
	i2c0Mutex = malloc(sizeof(shared_mutex_t));
	shared_mutex_t tmp = shared_mutex_init((char*)"/i2c0", mcp23017_data_len);
	memcpy(i2c0Mutex, &tmp, sizeof(shared_mutex_t));

	pid = getpid();
}

void spi0_mutex_init() {
	spi0Mutex = malloc(sizeof(shared_mutex_t));
	shared_mutex_t tmp = shared_mutex_init((char*)"/spi0", 0);
	memcpy(spi0Mutex, &tmp, sizeof(shared_mutex_t));

	pid = getpid();
}

void i2c0Lock() {
	if (!i2c0Mutex) {
		i2c0_mutex_init();
	}
	if (!mcp23017_node && !mcp23017_node_searched) {
		mcp23017_node = wiringPiFindNode(mcp23017_pin_base);
	}
	pthread_mutex_lock(i2c0Mutex->ptr);
	usingI2C0Mutex = 1;

	// load shared data
	if (pid != *i2c0Mutex->pid) {
		if (*i2c0Mutex->data_init && mcp23017_node) {
			memcpy(&mcp23017_node->data2, i2c0Mutex->data, mcp23017_data_len);
		}
		*i2c0Mutex->pid = pid;
	}
}

void i2c0Unlock() {
	if (usingI2C0Mutex) {
		usingI2C0Mutex = 0;

		if (!mcp23017_node && !mcp23017_node_searched) {
			mcp23017_node = wiringPiFindNode(mcp23017_pin_base);
			mcp23017_node_searched = 1;
		}

		// store shared data
		if (mcp23017_node) {
			memcpy(i2c0Mutex->data, &mcp23017_node->data2, mcp23017_data_len);
		}
		*i2c0Mutex->data_init = 1;

		pthread_mutex_unlock(i2c0Mutex->ptr);
	}
}

void spi0Lock() {
	if (!spi0Mutex) {
		spi0_mutex_init();
	}
	pthread_mutex_lock(spi0Mutex->ptr);
	usingSPI0Mutex = 1;
}

void spi0Unlock() {
	if (usingSPI0Mutex) {
		usingSPI0Mutex = 0;
		pthread_mutex_unlock(spi0Mutex->ptr);
	}
}
