#pragma once

class Interface1 {
public:
	virtual ~Interface1() = default;
	virtual double ComputeSalary(double hours) const = 0;
};

class Interface2 {
public:
	virtual ~Interface2() = default;
	virtual double ComputeHours(double hoursPerDay, int days) const = 0;
};