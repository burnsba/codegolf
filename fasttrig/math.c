#include <stdio.h>
#include <stdlib.h>

#define PI               3.141592653589793238462643383279502884197169399375105
#define PI_OVER_2        1.570796326794896619231321691639751442098584699687552
#define PI_OVER_180      0.017453292519943295769236907684886127134428718885417

static double minimaxsin(double x)
{
    // http://lolengine.net/blog/2011/12/21/better-function-approximations
    static const
    double a0 =  1.0,
           a1 = -1.666666666640169148537065260055e-1,
           a2 =  8.333333316490113523036717102793e-3,
           a3 = -1.984126600659171392655484413285e-4,
           a4 =  2.755690114917374804474016589137e-6,
           a5 = -2.502845227292692953118686710787e-8,
           a6 =  1.538730635926417598443354215485e-10;
    double x2 = x * x;
    return x * (a0 + x2 * (a1 + x2 * (a2 + x2
             * (a3 + x2 * (a4 + x2 * (a5 + x2 * a6))))));
}

int main()
{
	double input;
	double sin, cos;
	
	char line [15];
	
	FILE *ifp = fopen("trig.in", "r");
	FILE *ofp = fopen("trig.out", "w");
	
	while (fgets (line, 15, ifp))
	{
		input = atof(line);
		sin = minimaxsin(input * PI_OVER_180);
		cos = minimaxsin(PI_OVER_2 - input * PI_OVER_180);
		if (cos < 0.000000001 && cos > -0.000000001)
		{
			fprintf(ofp, "%.7f %.7f n\n", sin, cos);
		}
		else
		{
			fprintf(ofp, "%.7f %.7f %.7f\n", sin, cos, (sin/cos));
		}
	}
	
	fclose(ifp);
	fclose(ofp);

	return 0;
}