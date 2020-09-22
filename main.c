/**<
MIT License

Copyright (c) 2020 lunderwood-code

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct ResistorPair
{
    float R1;
    float R2;

    float SeriesValue;
    float ParallelValue;
};

struct PairAnalysis
{
    float DesiredValue;

    bool SeriesMatch;
    bool ParallelMatch;

    struct ResistorPair Pair;

    float SeriesPercentageError;
    float ParallelPercentageError;
};

struct ResistorPairAsString
{
    char R1[BUFSIZ];
    char R2[BUFSIZ];

    char SeriesValue[BUFSIZ];
    char ParallelValue[BUFSIZ];
};

struct SingleAnalysis
{
    float DesiredValue;

    bool Match;

    float R1;

    float PercentageError;
};

struct SingleResistorAsString
{
    char R1[BUFSIZ];
};


// Constructors
void NewResistorPair(struct ResistorPair *pair, float r1, float r2);

void NewPairAnalysis(struct PairAnalysis *analysis, struct ResistorPair pair, float desiredValue);
void NewResistorPairAsString(struct ResistorPairAsString *pairAsString, struct PairAnalysis analysis);

void NewSingleAnalysis(struct SingleAnalysis *analysis, float r1, float desiredValue);
void NewSingleResistorAsString(struct SingleResistorAsString *singleAsString, struct SingleAnalysis analysis);


// Helper functions
static void examinePairValues(const float r1, const float r2, const float desiredValue);
static void examineSingleValue(const float r1, const float desiredValue);
static void reportPairAnalysis(struct PairAnalysis analysis);
static void reportSingleAnalysis(struct SingleAnalysis analysis);
static void convertToStandardResFormat(float inputValue, char s[]);

static float getDesiredValue(void);


// Constants
const float ResistorsE24[] = {1.0f, 1.2f, 1.5f, 1.8f, 2.2f, 2.7f, 3.3f, 3.9f, 4.7f, 5.6f, 6.8f, 8.2f};
const float ValueMultipliers[] = { 1.0f, 10.0f, 100.0f, 1000.0f, 10000.0f, 100000.0f, 1000000.0f};


/** \brief Gets desired value and examines the values
 *
 * \param void
 * \return int
 *
 */
int main(void)
{
    unsigned int iR1, iR2;
    size_t sizeResistorsE24 = sizeof(ResistorsE24)/sizeof(ResistorsE24[0]);

    float desiredValue;

    puts("*** Calculator for 2 resistor pair ***");

    desiredValue = getDesiredValue();

    puts("\n\nResults within 2% of value...");

    for(iR1=0; iR1 < sizeResistorsE24; iR1++)
    {
        examineSingleValue(ResistorsE24[iR1], desiredValue);

        for(iR2=iR1; iR2 < sizeResistorsE24; iR2++)
        {
            examinePairValues(ResistorsE24[iR1], ResistorsE24[iR2], desiredValue);
        }
    }


    puts("\nPress enter to continue...");
    getchar();

    return EXIT_SUCCESS;
}

/** \brief Established values for a ResistorPair struct
 *
 * \param pair struct ResistorPair*
 * \param r1 float
 * \param r2 float
 * \return void
 *
 */
void NewResistorPair(struct ResistorPair *pair, float r1, float r2)
{
    pair -> R1 = r1;
    pair -> R2 = r2;
    pair -> SeriesValue = r1 + r2;
    pair -> ParallelValue = (r1*r2)/(r1 + r2);
}

/** \brief Establishes values for a PairAnalysis struct
 *
 * \param analysis struct PairAnalysis*
 * \param pair struct ResistorPair
 * \param desiredValue float
 * \return void
 *
 */
void NewPairAnalysis(struct PairAnalysis *analysis, struct ResistorPair pair, float desiredValue)
{
    float seriesPercentage = (100 * pair.SeriesValue) / desiredValue;
    float parallelPercentage = (100 * pair.ParallelValue) / desiredValue;

    float upperBound = desiredValue * 1.02;
    float lowerBound = desiredValue * 0.98;

    analysis -> Pair = pair;
    analysis -> DesiredValue = desiredValue;

    analysis -> SeriesMatch = (lowerBound < pair.SeriesValue) && (pair.SeriesValue < upperBound) ? true:false;
    analysis -> ParallelMatch = (lowerBound < pair.ParallelValue) && (pair.ParallelValue < upperBound) ? true:false;

    // Adjust for error
    analysis -> SeriesPercentageError = (seriesPercentage > 100) ? seriesPercentage - 100 : 100 - seriesPercentage;
    analysis -> ParallelPercentageError = (parallelPercentage > 100) ? parallelPercentage - 100 : 100 - parallelPercentage;

}

/** \brief Establishes values for a ResistorPairAsString struct
 *
 * \param pairAsString struct ResistorPairAsString*
 * \param analysis struct PairAnalysis
 * \return void
 *
 */
void NewResistorPairAsString(struct ResistorPairAsString *pairAsString, struct PairAnalysis analysis)
{
    char r1AsString[BUFSIZ];
    char r2AsString[BUFSIZ];
    char seriesAsString[BUFSIZ];
    char parallelAsString[BUFSIZ];

    convertToStandardResFormat(analysis.Pair.R1, r1AsString);
    convertToStandardResFormat(analysis.Pair.R2, r2AsString);
    sprintf(seriesAsString, "%f", analysis.Pair.SeriesValue);
    sprintf(parallelAsString, "%f", analysis.Pair.ParallelValue);

    strncpy(pairAsString->R1, r1AsString, BUFSIZ);
    strncpy(pairAsString->R2, r2AsString, BUFSIZ);
    strncpy(pairAsString->SeriesValue, seriesAsString, BUFSIZ);
    strncpy(pairAsString->ParallelValue, parallelAsString, BUFSIZ);
}


/** \brief Establishes values for a SingleAnalysis struct
 *
 * \param analysis struct SingleAnalysis*
 * \param r1 float
 * \param desiredValue float
 * \return void
 *
 */
void NewSingleAnalysis(struct SingleAnalysis *analysis, float r1,  float desiredValue)
{
    float percentage = (100 * r1) / desiredValue;

    float upperBound = desiredValue * 1.02;
    float lowerBound = desiredValue * 0.98;

    analysis -> R1 = r1;
    analysis -> DesiredValue = desiredValue;
    analysis -> Match = (lowerBound < r1) && (r1 < upperBound) ? true:false;

    // Adjust for error
    analysis -> PercentageError = (percentage > 100) ? percentage - 100 : 100 - percentage;

}

/** \brief Establishes values for a SingleResistorAsString struct
 *
 * \param singleAsString struct SingleResistorAsString*
 * \param analysis struct SingleAnalysis
 * \return void
 *
 */
void NewSingleResistorAsString(struct SingleResistorAsString *singleAsString, struct SingleAnalysis analysis)
{
    char r1AsString[BUFSIZ];

    convertToStandardResFormat(analysis.R1, r1AsString);

    strncpy(singleAsString->R1, r1AsString, BUFSIZ);
}

/** \brief Control of how the PairAnalysis is displayed - Currently only reports matches
 *
 * \param analysis struct PairAnalysis
 * \return void
 *
 */
static void reportPairAnalysis(struct PairAnalysis analysis)
{
    struct ResistorPairAsString pairAsString;

    if(analysis.SeriesMatch)
    {
        NewResistorPairAsString(&pairAsString, analysis);

        printf("Series:\t\t%s\t%s\t%s\t%3.1f%%\n", pairAsString.R1, pairAsString.R2, pairAsString.SeriesValue, analysis.SeriesPercentageError);
    }

    if(analysis.ParallelMatch)
    {
        NewResistorPairAsString(&pairAsString, analysis);

        printf("Parallel:\t%s\t%s\t%s\t%3.1f%%\n", pairAsString.R1, pairAsString.R2, pairAsString.ParallelValue, analysis.ParallelPercentageError);
    }
}

/** \brief Control of how the SingleAnalysis is displayed - Currently only reports matches
 *
 * \param analysis struct SingleAnalysis
 * \return void
 *
 */
static void reportSingleAnalysis(struct SingleAnalysis analysis)
{
    struct SingleResistorAsString singleAsString;

    if(analysis.Match)
    {
        NewSingleResistorAsString(&singleAsString, analysis);

        printf("Single:\t\t%s\t\t\t\t%3.1f%%\n", singleAsString.R1, analysis.PercentageError);
    }

}

/** \brief Given 2 resistor values and a desired value, this function sees if they can create a value within 2%
 *
 * \param r1 const float
 * \param r2 const float
 * \param desiredValue const float
 * \return void
 *
 */
static void examinePairValues(const float r1, const float r2, const float desiredValue)
{
    unsigned int iR1=0, iR2=0;
    size_t sizeValueMultipliers = sizeof(ValueMultipliers)/ sizeof(ValueMultipliers[0]);

    for (iR1=0; iR1<sizeValueMultipliers; iR1++)
    {
        for (iR2=iR1; iR2<sizeValueMultipliers; iR2++)
        {
            struct ResistorPair pair;
            struct PairAnalysis analysis;

            NewResistorPair(&pair, r1 * ValueMultipliers[iR1], r2 * ValueMultipliers[iR2]);

            NewPairAnalysis(&analysis, pair, desiredValue);

            reportPairAnalysis(analysis);

        }
    }
}


/** \brief Given a resistor value and a desired value, this function sees if it is within 2%
 *
 * \param r1 const float
 * \param desiredValue const float
 * \return void
 *
 */
static void examineSingleValue(const float r1, const float desiredValue)
{
    unsigned int iR1=0;
    size_t sizeValueMultipliers = sizeof(ValueMultipliers)/ sizeof(ValueMultipliers[0]);

    for (iR1=0; iR1<sizeValueMultipliers; iR1++)
    {
        struct SingleAnalysis analysis;

        NewSingleAnalysis(&analysis, r1 * ValueMultipliers[iR1], desiredValue);

        reportSingleAnalysis(analysis);

    }
}


/** \brief Changes a decimal value to a standard resistor format - 8.2 ohm to 8R2, 1k, 2.2M, ect...
 *
 * \param inputValue float
 * \param s[] char
 * \return void
 *
 */
static void convertToStandardResFormat(float inputValue, char s[])
{
    if(inputValue < 1)
    {
        inputValue *= 1000;
        sprintf(s, "%.0f", inputValue);
        strcat(s, "m");
    }
    else if(inputValue<10)
    {
        sprintf(s, "%.1f", inputValue);
        s[strcspn(s, ".")] = 'R';
    }
    else if(inputValue<1000)
    {
        sprintf(s, "%.0f", inputValue);
        strcat(s, "R");
    }
    else if(inputValue<10000)
    {
        inputValue /= 1000;
        sprintf(s, "%.1f", inputValue);
        s[strcspn(s, ".")] = 'k';
    }
    else if(inputValue<1000000)
    {
        inputValue /= 1000;
        sprintf(s, "%.0f", inputValue);
        strcat(s, "k");
    }
    else if(inputValue<10000000)
    {
        inputValue /= 1000000;
        sprintf(s, "%.1f", inputValue);
        s[strcspn(s, ".")] = 'M';
    }
    else
    {
        inputValue /= 1000000;
        sprintf(s, "%.1f", inputValue);
        strcat(s, "M");
    }
}


/** \brief Gets desired value from user, making sure that input is valid
 *
 * \param void
 * \return float
 *
 */
static float getDesiredValue(void)
{
    float desiredValue;
    int numberOfValidInputs;

    do
    {
        int ch;

        printf("\nEnter desired value: ");
        fflush(stdout);
        numberOfValidInputs = scanf("%f", &desiredValue);

        if (numberOfValidInputs != 1)
        {
            puts("\n**Error: please enter a valid number**");
        }

        // Remove all input from stdin
        while(('\n' != (ch = getchar())) && ch != EOF)
            continue;

    }
    while(numberOfValidInputs != 1);

    return desiredValue;
}


