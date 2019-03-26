#include <pspkernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


int ReadFile(const char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);
	
	sceIoClose(fd);
	return read;
}

int WriteFile(const char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);
	return written;
}

int limit(int val,int min,int max)
{
	if(val<min) val = max;
	if(val>max) val = min;
	return val;
}

int limit2(int val,int min,int max)
{
	if(val<min) val = min;
	if(val>max) val = max;
	return val;
}

int itoa(int n, char* buf )
{
	int p = 0, r;

	if ( n < 0 ) { buf[ 0 ] = '-'; n = -n; p++ ;}

    // 桁数分のポインタを進める
    int t = n;
    while ( t /= 10 ) p++;
    
	r = p;
    // 終端を付加
    buf[ p + 1 ] = '\0';

    // 桁数分を書き込む
    do
        buf[ p-- ] = n % 10 + '0';
    while ( n /= 10 );

	return r;
}

int my_vsprintf(char *out,const char *format, va_list arg)
{
	char *str;
	char *s = out;
	char *p = (char *)format;
	va_list ap = arg;
	int count = 0;

	char dbuf[11];


	while ( *p )
	{

		if( *p != '%' )
		{
			*s++ = *p++;
			count++;
		}

		if( *p == '%' ) 
		{
			p++;

			switch ( *p )
			{
			case 's':
				p++;
				str = va_arg ( ap, char*);

				while( *str ) {
					*s++ = *str++;
					count++;
				}
				break;

			case 'd':
				 p++;
				 itoa( va_arg ( ap , int ), dbuf );
				 str = dbuf;
				 while ( *str ) {
					 *s++ = *str++;
					 count++;
				 }
				 break;
			case 'c':
				p++;
				*s++ = va_arg ( ap, int );
				count++;
				break;

			default:
				*s++ = '%';
				count++;
				break;
			}

		}

	}

	*s = '\0';
	return count;

}

int my_sprintf(char *s, const char *format, ... )
{
	va_list ap;
	int count;

	va_start ( ap, format );
	count = my_vsprintf( s , format , ap);
	va_end ( ap );
	return count;
}

static int my_strcspn(const char *s1, const char *s2)
{
	const char *p = s1;

    for ( ; *s1; s1++) 
	{
        const char  *t;

        for (t = s2; *t; t++)
            if (*t == *s1)
                return (int)(s1 - p);
    }

	return (int)(s1 - p);
}

static int my_strspn(const char *s1, const char *s2)
{
    const char *p = s1;

    for ( ; *s1; s1++)
	{
        const char  *t;

        for (t = s2; *t != *s1; t++)
            if (*t == '\0')
                return (int)(s1 - p);
    }
    return (int)(s1 - p);
}

char *my_strtok_r(char *s1, const char *s2 , char **s3 )
{
    char  *pbegin, *pend;
//    static char  *save = "";
	char *save = *s3;

    pbegin = s1 ? s1 : save;
	if(!pbegin)
		return NULL;

    pbegin += my_strspn(pbegin, s2);               /* strspnを利用 */
    if (*pbegin == '\0')
	{
        *s3 = NULL;
        //save = "";
        return (NULL);
    }

    pend = pbegin + my_strcspn(pbegin, s2);        /* strcspnを利用 */
    if (*pend != '\0')
	{
		*pend = '\0';	
		pend++;
	}
	
	*s3 = pend;
	//save = pend;
    return (pbegin);
}
