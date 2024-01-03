using System;

class Program
{
    static void Main()
    {
        float floatValue;

        // Read float variable from console
        Console.WriteLine("Enter a float value:");
        if (!float.TryParse(Console.ReadLine(), out floatValue))
        {
            Console.WriteLine("Invalid input. Please enter a valid float value.");
        }
        else
        {
            // Print the value of the float variable
            Console.WriteLine($"The entered float value is: {floatValue}");
        }
    }
}
