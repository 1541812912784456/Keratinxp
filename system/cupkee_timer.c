/*
MIT License

This file is part of cupkee project.

Copyright (c) 2016-2017 Lixing Ding <ding.lixing@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "cupkee.h"

typedef struct cupkee_timer_t {
    uint8_t inst;
    uint32_t duration;

    cupkee_cb_t cb;
    intptr_t    cb_param;
} cupkee_timer_t;

static int timer_object_tag = -1;

int cupkee_timer_setup(void)
{
    if (0 > (timer_object_tag = cupkee_object_register(sizeof(cupkee_timer_t), NULL))) {
        return -1;
    }

    return 0;
}

int cupkee_timer_start(int us, cupkee_cb_t cb, intptr_t param)
{
    cupkee_timer_t *timer;
    int8_t inst;
    int id;

    inst = hw_timer_alloc();
    if (inst < 0) {
        return -CUPKEE_ERESOURCE;
    }

    id = cupkee_object_alloc(timer_object_tag);
    if (id < 0 || NULL == (timer = CUPKEE_OBJECT(id, cupkee_timer_t))) {
        hw_timer_release(inst);
        return -CUPKEE_ENOMEM;
    }

    timer->inst = inst;
    timer->cb = cb;
    timer->cb_param = param;
    timer->duration = us;

    hw_timer_start(inst, id, us);

    return id;
}

static inline cupkee_timer_t *timer_from_object(int id) {
    if (timer_object_tag != cupkee_object_tag(id)) {
        return CUPKEE_OBJECT(id, cupkee_timer_t);
    }
    return NULL;
}

#define CUPKEE_OBJECT_HAS_TAG(id, tag) (cupkee_object_tag(id) == (tag))

int cupkee_timer_duration(int id)
{
    cupkee_timer_t *timer;

    if (NULL == (timer = timer_from_object(id))) {
        return -CUPKEE_EINVAL;
    }

    return hw_timer_duration_get(timer->inst);
}

int cupkee_timer_stop(int id)
{
    cupkee_timer_t *timer;

    if (NULL == (timer = timer_from_object(id))) {
        return -CUPKEE_EINVAL;
    }

    return hw_timer_stop(timer->inst);
}


