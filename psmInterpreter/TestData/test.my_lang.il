.assembly extern mscorlib { }
.assembly Example { }
.module Example.exe
.method static void Main() cil managed
{
	.entrypoint
	.locals init (
		float32 c,
		float32 v,
		float32 i
	)
	ldstr "Hello world"
	call void [mscorlib]System.Console::Write(string)
	ldloca c
	ldc.i4 1
	conv.r4
	ldc.i4 3
	conv.r4
	ldc.i4 2
	conv.r4
	ldc.i4 2
	conv.r4
	call float64 [mscorlib]System.Math::Pow(float64, float64) 
	call float64 [mscorlib]System.Math::Pow(float64, float64) 
	add
	ldc.i4 6
	conv.r4
	ldc.i4 7
	conv.r4
	mul
	add
	stind.r4
	call string [mscorlib]System.Console::ReadLine()
	call float32 [mscorlib]System.Single::Parse(string)
	stloc v
	ldloc v
	ldc.i4 0
	conv.r4
	cgt
	brfalse m1
	ldloc v
	call void [mscorlib]System.Console::WriteLine(float32)
	br m2
	m1:
	ldc.i4 0
	conv.r4
	call void [mscorlib]System.Console::WriteLine(float32)
	m2:
	ldloca i
	ldc.i4 0
	conv.r4
	stind.r4
	m3:
	ldloc i
	ldc.i4 5
	conv.r4
	clt
	brfalse m4
	br m5
	m6:
	ldloca i
	ldloc i
	ldc.i4 1
	conv.r4
	add
	stind.r4
	br m3
	m5:
	ldloc i
	ldloc v
	add
	call void [mscorlib]System.Console::WriteLine(float32)
	br m6
	m4:
	ldloc c
	call void [mscorlib]System.Console::WriteLine(float32)
	ret
}
