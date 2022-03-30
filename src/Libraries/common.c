#include "common.h"
#include <stdarg.h>
//#include "stdio.h"

#define PADDING_RIGHT   1
#define PADDING_ZERO    2
#define PRINTI_BUF_LEN  12      //for 32bit signed integer

int print2(char **out, int ch)
{
    int count = 1;

    if (ch == '\n')
        count += print2(out, '\r');

    if (out)
        *(*out)++ = ch;
    else
        ;
//        putchar(ch);

    return count;
}

int prints2(char **out, const char *str, int width, int pad)
{
    int count = 0, padChar = ' ';

    if (width > 0)
    {
        int len = 0;
        const char *ptr;

        for (ptr = str; *ptr; ++ptr)
            len++;

        if (len >= width)
            width = 0;
        else
            width -= len;

        if (pad & PADDING_ZERO)
            padChar = '0';
    }

    if (!(pad & PADDING_RIGHT))
    {
        for (; width > 0; width--)
        {
            count += print2(out, padChar);
        }
    }

    while (*str)
    {
        count += print2(out, *str++);
    }

    while (width--)
    {
        count += print2(out, padChar);
    }

    return count;
}

static char buf[PRINTI_BUF_LEN];
int printi2(char **out, int val, int base, int sign, int width, int pad,
            int alphaBase)
{

    char *ptr;
    int t, neg = 0, count = 0;
    unsigned int uval = (unsigned int) val;

    if (val == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return prints2(out, buf, width, pad);
    }

    if (sign && base == 10 && val < 0)
    {
        neg = 1;
        uval = (unsigned int) -val;
    }

    ptr = buf + PRINTI_BUF_LEN - 1;
    *ptr = '\0';

    while (uval)
    {
        t = uval % base;
        if (t >= 10)
            t += alphaBase - '0' - 10;
        *--ptr = t + '0';
        uval /= base;
    }

    if (neg)
    {
        if (width && (pad & PADDING_ZERO))
        {
            count += print2(out, '-');
            width--;
        }
        else
        {
            *--ptr = '-';
        }
    }

    return count + prints2(out, ptr, width, pad);
}

int vprintf2(char **out, const char *fmt, va_list args)
{
    int count = 0;

    while (*fmt)
    {
        if (*fmt == '%')
        {
            int width = 0, pad = 0;
            int type;
            char *str, tmp[2];

            fmt++;

            if (!*fmt)
                break;

            if (*fmt == '%')
                goto _out;

            if (*fmt == '-')
            {
                pad = PADDING_RIGHT;
                fmt++;
            }

            if (*fmt == '0')
            {
                pad = PADDING_ZERO;
                fmt++;
            }

            for (; *fmt >= '0' && *fmt <= '9'; fmt++)
                width = width * 10 + *fmt - '0';

            type = *fmt;
            switch (type)
            {
            case 's':
                str = va_arg(args, char *);
                str = str ? str : "(null)";
                count += prints2(out, str, width, pad);
                break;

            case 'd':
                count += printi2(out, va_arg(args, int), 10, 1, width, pad, 0);
                break;

            case 'u':
                count += printi2(out, va_arg(args, int), 10, 0, width, pad, 0);
                break;

            case 'x':
                count += printi2(out, va_arg(args, int), 16, 0, width, pad, 'a');
                break;

            case 'p':
                count += prints2(out, "0x", 0, 0);
                width = 8;
                pad = PADDING_ZERO;
            case 'X':
                count += printi2(out, va_arg(args, int), 16, 0, width, pad, 'A');
                break;

            case 'c':
                tmp[0] = (char)va_arg(args, int);
                tmp[1] = 0;
                count += prints2(out, tmp, width, pad);
                break;
            }
        }
        else
        {
            _out: count += print2(out, *fmt);
        }

        fmt++;
    }

    if (out)
        **out = '\0';

    va_end(args);
    return count;
}

int printf2(const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    return vprintf2(0, fmt, va);
}

int sprintf2(char *buf, const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    return vprintf2(&buf, fmt, va);
}
