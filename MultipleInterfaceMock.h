#pragma once

#include "Interfaces.h"
#include "Mockingbird.hpp"

START_MOCK(CompensationMock, Interface1, Interface2)
FUNCTION(ComputeSalary, double, (double hours), const, override, return 0.0, hours)
FUNCTION(ComputeHours, double, (double hoursPerDay, int days), const, override, return 0.0, hoursPerDay, days)
END_MOCK(CompensationMock)

