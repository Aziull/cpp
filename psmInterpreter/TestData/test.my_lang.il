.assembly extern mscorlib { }
.assembly Example { }
.module Example.exe
.method static void Main() cil managed
{
	.entrypoint
	.locals init (
		float32 c,
		float32 i
	)
	ldstr "Hello world"
	call void [mscorlib]System.Console::Write(string)
	ldloca c
	ldc.i4 1
	conv.r4
	stind.r4
	ldloc c
	call void [mscorlib]System.Console::WriteLine(float32)
	call string [mscorlib]System.Console::ReadLine()
	call float32 [mscorlib]System.Single::Parse(string)
	stloc c
	ldloc c
	call void [mscorlib]System.Console::WriteLine(float32)
	ldloc c
	ldc.i4 0
	conv.r4
	ceq
	brfalse m1
	ldc.i4 0
	conv.r4
	call void [mscorlib]System.Console::WriteLine(float32)
	br m2
	m1:
	ldloc c
	call void [mscorlib]System.Console::WriteLine(float32)
	m2:
	ldloc c
	ldc.i4 1
	conv.r4
	cgt
	brfalse m3
	ldc.i4 1
	conv.r4
	call void [mscorlib]System.Console::WriteLine(float32)
	br m4
	m3:
	m4:
	ldloc c
	ldc.i4 2
	conv.r4
	cge
	brfalse m5
	ldc.i4 2
	conv.r4
	call void [mscorlib]System.Console::WriteLine(float32)
	br m6
	m5:
	m6:
	ldloca i
	ldc.i4 0
	conv.r4
	stind.r4
	m7:
	ldloc i
	ldc.i4 5
	conv.r4
	clt
	brfalse m8
	br m9
	m10:
	ldloca i
	ldloc i
	ldc.i4 1
	conv.r4
	add
	stind.r4
	br m7
	m9:
	ldloc i
	call void [mscorlib]System.Console::WriteLine(float32)
	br m10
	m8:
	ret
}
