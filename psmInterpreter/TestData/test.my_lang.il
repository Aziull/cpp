// Referenced Assemblies.
.assembly extern mscorlib
{
	.publickeytoken = (B7 7A 5C 56 19 34 E0 89 )
}
// Out assembly.
.assembly test1
{
	.hash algorithm 0x00008004
	.ver 0:0:0:0
}
.module test1.exe
// Definition of Program class.
.class private auto ansi beforefieldinit Program
	extends [mscorlib]System.Object
{
	.method private hidebysig static void Main(string[] args) cil managed
	{
		.locals (
		)
		.entrypoint
		ldstr "Hello world"
		call void [mscorlib]System.Console::Write(string)
	}
}
