cbgt create root table root with path /home/ezhocha/hsbgt on tcid 10.10.10.1 rank 1 at console

cbgt create user table hansoul column family colf-0:colf-1:colf-2 from path /home/ezhocha/hsbgt at console

cbgt insert user table hansoul row-A:colf-0:colq-0-0:.a.b.c.d:val-0-0-0 from path /home/ezhocha/hsbgt at console
cbgt insert user table hansoul row-A:colf-0:colq-0-1:.a.b.c.d:val-0-0-1 from path /home/ezhocha/hsbgt at console
cbgt insert user table hansoul row-A:colf-0:colq-0-2:.a.b.c.d:val-0-0-2 from path /home/ezhocha/hsbgt at console
cbgt insert user table hansoul row-A:colf-0:colq-0-3:.a.b.c.d:val-0-0-3 from path /home/ezhocha/hsbgt at console

cbgt show module on all at console
cbgt debug split on tcid 10.10.10.1 rank 1 modi 3 at console
cbgt show module on all at console

cbgt fetch user table hansoul row-A:colf-0:colq-0-0:.a.b.c.d from path /home/ezhocha/hsbgt at console
cbgt fetch user table hansoul row-A:colf-0:colq-0-1:.a.b.c.d from path /home/ezhocha/hsbgt at console
cbgt fetch user table hansoul row-A:colf-0:colq-0-2:.a.b.c.d from path /home/ezhocha/hsbgt at console
cbgt fetch user table hansoul row-A:colf-0:colq-0-3:.a.b.c.d from path /home/ezhocha/hsbgt at console

cbgt runthrough depth on tcid 10.10.10.1 rank 1 modi 0 at console

cbgt debug merge on tcid 10.10.10.1 rank 1 modi 2 at console
cbgt show module on all at console

cbgt fetch user table hansoul row-A:colf-0:colq-0-0:.a.b.c.d from path /home/ezhocha/hsbgt at console
cbgt fetch user table hansoul row-A:colf-0:colq-0-1:.a.b.c.d from path /home/ezhocha/hsbgt at console
cbgt fetch user table hansoul row-A:colf-0:colq-0-2:.a.b.c.d from path /home/ezhocha/hsbgt at console
cbgt fetch user table hansoul row-A:colf-0:colq-0-3:.a.b.c.d from path /home/ezhocha/hsbgt at console

cbgt runthrough depth on tcid 10.10.10.1 rank 1 modi 0 at console

cbgt delete user table hansoul row-A:colf-0:colq-0-1:.a.b.c.d from path /home/ezhocha/hsbgt at console

cbgt show module on all at console
cbgt debug split on tcid 10.10.10.1 rank 1 modi 10 at console

cbgt fetch user table hansoul row-A:colf-0:colq-0-0:.a.b.c.d from path /home/ezhocha/hsbgt at console
cbgt fetch user table hansoul row-A:colf-0:colq-0-2:.a.b.c.d from path /home/ezhocha/hsbgt at console
cbgt fetch user table hansoul row-A:colf-0:colq-0-3:.a.b.c.d from path /home/ezhocha/hsbgt at console
