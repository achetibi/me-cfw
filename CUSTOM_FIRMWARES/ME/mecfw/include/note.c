

int sceKernelBootFrom()
{
	int api = get();
	if( api < 0x146)
	{
		if( api < 0x144)
		{
			if( api < 0x134)
			{
				if(api < 0x130 )
				{
					if( api >= 0x110 )
					{
						if( api < 0x11A)
						{
							return 0x20;
						}
						else
						{
							//loc_00004D34
							if( ( api - 0x120 ) < 7 )
								return 0x20;
							else
								return 0;
							
						}
					}
					else
					{
						//loc_00004D24
						return 0;
					}
				}
				else
				{
					//loc_00004D24
					return 0x30;
				}
			}
			else
			{
				//loc_00004D44
				if( api < 0x140 )
				{
					return 0x40;
				}
				else
				{
					if( api < 0x143)
						return 0x40;
					
					if( strncmp( sceKernelInitFileName(), "flash3:", 7 ) == 0 )
						return 0x80;
					
					return 0x40;
				}
			}

		}
		else
		{
			//loc_00004D24
			return 0x40;
		}
		
	}
	else
	{
		//loc_00004D84
		if( api <  0x157 )
		{
			if( api < 0x155)
			{
				if( api < 0x151)
					return 0;
				
				if( api < 0x154)
				{
					return 0x50;
				}
				else
				{
					return 0x50;
				}
				
			}
			else
			{
				return 0x50;
			}
		}
		else
		{
			//loc_00004DBC
			if( api < 0x172)
			{
				if( api >= 0x170 )
					return 0x20;
				
				if( (api - 0x160) < 2)
					return 0x20;
				
				return 0;
			}
			else
			{
				return 0;
			}
		}
	}
	
}

int strmatch(const char *s, const char *t)
{
	char c1, c2;
	while( ((c1 = *s) & (c2 = *t)) != 0 )
	{	
		if( c1 != '?' )
		{
			if( c1 != c2)
			{
				break;
			}
			else
			{	
				return 0;
			}
		}

		s++;
		t++;
	}

	return ( c1 - c2 );
}