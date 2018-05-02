#define BUF_SIZE 1000
int pos[3];//player pos x,y,z
void setBall()
{
	int x=0,y=0,z=0;
	int i;
	int block_pos[4];
	int start_pos[3];
	char msgs[BUF_SIZE];
	int r=5;
	getPose();
	for(i=0;i<3;i++) start_pos[i]=pos[i];
	start_pos[2]+=10;
	start_pos[1]+=r;
	for(x=0;x<r*r;x++)
	{
		for(y=0;y<r*r;y++)
		{
			for(z=0;z<r*r;z++)
			{
				if(((x*x)+(y*y)+(z*z))<=(r*r))
				{
					block_pos[0]=start_pos[0]-x;
					block_pos[1]=start_pos[1]+y;
					block_pos[2]=start_pos[2]-z;
					block_pos[3]=2;
					strcpy(msgs,"world.setBlock(");
					setToServer(msgs,block_pos,4);
					block_pos[0]=start_pos[0]-x;
					block_pos[1]=start_pos[1]-y;
					block_pos[2]=start_pos[2]+z;
					block_pos[3]=2;
					strcpy(msgs,"world.setBlock(");
					setToServer(msgs,block_pos,4);
					block_pos[0]=start_pos[0]-x;
					block_pos[1]=start_pos[1]-y;
					block_pos[2]=start_pos[2]-z;
					block_pos[3]=2;
					strcpy(msgs,"world.setBlock(");
					setToServer(msgs,block_pos,4);
					block_pos[0]=start_pos[0]+x;
					block_pos[1]=start_pos[1]-y;
					block_pos[2]=start_pos[2]-z;
					block_pos[3]=2;
					strcpy(msgs,"world.setBlock(");
					setToServer(msgs,block_pos,4);
					block_pos[0]=start_pos[0]+x;
					block_pos[1]=start_pos[1]+y;
					block_pos[2]=start_pos[2]-z;
					block_pos[3]=2;
					strcpy(msgs,"world.setBlock(");
					setToServer(msgs,block_pos,4);
					block_pos[0]=start_pos[0]+x;
					block_pos[1]=start_pos[1]-y;
					block_pos[2]=start_pos[2]+z;
					block_pos[3]=2;
					strcpy(msgs,"world.setBlock(");
					setToServer(msgs,block_pos,4);
					block_pos[0]=start_pos[0]-x;
					block_pos[1]=start_pos[1]+y;
					block_pos[2]=start_pos[2]+z;
					block_pos[3]=2;
					strcpy(msgs,"world.setBlock(");
					setToServer(msgs,block_pos,4);
					block_pos[0]=x+start_pos[0];
					block_pos[1]=y+start_pos[1];
					block_pos[2]=z+start_pos[2];
					block_pos[3]=2;
					strcpy(msgs,"world.setBlock(");
					setToServer(msgs,block_pos,4);
				}
			}
		}
	}
}
void setPyramid()
{
	int x=0,y=0,z=0;
	int i;
	int block_pos[4];
	int start_pos[3];
	char msgs[BUF_SIZE];
	int layer=10;
	getPose();
	for(i=0;i<3;i++) start_pos[i]=pos[i];
	start_pos[2]+=10;
	start_pos[1]+=layer;
	for(i=0;i<layer;i++)
	{
		y--;
		for(x=-i;x<=i;x++)
		{
			for(z=-i;z<=i;z++)
			{
				block_pos[0]=start_pos[0]+x;
				block_pos[1]=start_pos[1]+y;
				block_pos[2]=start_pos[2]+z;
				block_pos[3]=3;
				strcpy(msgs,"world.setBlock(");
				setToServer(msgs,block_pos,4);
			}
		}
	}
}
void setTrap()
{
	int x=0,y=0,z=0;
	int i;
	int block_pos[4];
	int start_pos[3];
	char msgs[BUF_SIZE];
	int layer=10;
	getPose();
	for(i=0;i<3;i++) start_pos[i]=pos[i];
	start_pos[2]+=1;
	start_pos[1]+=1;
	for(i=0;i<layer;i++)
	{
		y--;
		for(x=-i;x<=i;x++)
		{
			for(z=-i;z<=i;z++)
			{
				block_pos[0]=start_pos[0]+x;
				block_pos[1]=start_pos[1]+y;
				block_pos[2]=start_pos[2]+z;
				block_pos[3]=0;
				strcpy(msgs,"world.setBlock(");
				setToServer(msgs,block_pos,4);
			}
		}
	}
}
void setMaze()
{
	int x=0,y=0,z=0;
	int i;
	int block_pos[4];
	int start_pos[3];
	char msgs[BUF_SIZE];
	int layer=10;
	getPose();
	for(i=0;i<3;i++) start_pos[i]=pos[i];
	start_pos[2]+=1;
	start_pos[1]+=1;
	for(i=0;i<layer;i++)
	{
		y--;
		for(x=-i;x<=i;x++)
		{
			for(z=-i;z<=i;z++)
			{
				block_pos[0]=start_pos[0]+x;
				block_pos[1]=start_pos[1]+y;
				block_pos[2]=start_pos[2]+z;
				block_pos[3]=0;
				strcpy(msgs,"world.setBlock(");
				setToServer(msgs,block_pos,4);
			}
		}
	}
}
