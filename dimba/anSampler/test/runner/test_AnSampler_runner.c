/**
 *  \file       test_AnSampler_runner.c
 *  \brief      Test runner of analog signal sampler.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  LeFr  v1.0.00  ---
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "unity_fixture.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
TEST_GROUP_RUNNER(AnSampler)
{
	RUN_TEST_CASE(AnSampler, ClearAfterInit);
	RUN_TEST_CASE(AnSampler, FailsOneAnSamplerOnInit);
	RUN_TEST_CASE(AnSampler, StoreOneAnSample);
}

/* ------------------------------ End of file ------------------------------ */
