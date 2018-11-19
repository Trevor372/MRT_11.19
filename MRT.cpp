/*
Copyright (C) 2011 Georgia Institute of Technology

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A mARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
* Generates square pulse current commands.
*/

#include <MRT.h>
#include <cmath>

extern "C" Plugin::Object *createRTXIPlugin(void) {
	return new MRT();
}

static DefaultGUIModel::variable_t vars[] =
{
	{ "Vin", "", DefaultGUIModel::INPUT, },
	{ "Iout", "", DefaultGUIModel::OUTPUT, },
	{ "Stim time (ms)", "Amount of time current is injected at every step", DefaultGUIModel::PARAMETER
		| DefaultGUIModel::DOUBLE, },
	{ "Current Range Start (pA)", "Starting current of the steps",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Current Range End (pA)", "Ending current of the steps", DefaultGUIModel::PARAMETER
		| DefaultGUIModel::DOUBLE, },
	{ "Increment (pA)", "How much the current increases on each step",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Cycles (#)", "How many times to repeat the protocol",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER, },
	{ "Inter-Cycle-Interval (s)", "The time between each cycle where the protocol isn't running (Iout is 0)",
	  DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Downtime (s)", "The time between each step where the protocol isn't running (Iout is 0)",
	  DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	
	
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

MRT::MRT(void) : DefaultGUIModel("MRT", ::vars, ::num_vars), dt(RT::System::getInstance()->getPeriod() * 1e-6), period(0.250), rStart(-100), rEnd(380), step_size(20), Ncycles(1), ICI(5), Downtime(0.556) {
	setWhatsThis("<p><b>I-Step:</b><br>This module generates a series of currents in a designated range followed by a fixed maximum current.</p>");
	createGUI(vars, num_vars);
	update(INIT);
	refresh();
	resizeMe();
}

MRT::~MRT(void) {}

void MRT::execute(void) {
	V = input(0);  //V
	int down = 0;
	Iout = 0;
	if (cycle < Ncycles) // if the program isn't done cycling
 {
	double abs_anovershoot = abs(rEnd+step_size);
	double abs_rEnd = abs(rEnd);
	double abs_step = abs(step);
	double abs_rStart = abs(rStart);
	if (rStart > rEnd)
		{
			step_size = abs(step_size);
			step_size = -step_size;
			step = abs(step);
			step = -step;
		}
		if ( abs_step <= (abs_rEnd + abs_rStart)) // if the current output is less highest desired output
	        {
		        if (age < ((period/1000)  - EPS)) 
			// if the delay is over but not period
			{
				if (interage < ICI && cycle > 0) //if time isn't greater than the ICI
					{
				 	  Iout = 0;
					  interage += dt/1000;
					}
				else if (abs_step >= (abs_rEnd + abs_rStart) && interage >= ICI) 
					{
					  Iout = rEnd;
					  age += dt / 1000;
					  time = 0;
					}
				else 
					{
					  Iout = (Iout + rStart + step);
					  age += dt / 1000;
					  time = 0;
					}
			}
			else
			{
				age += dt/1000;
			}
			if (time < Downtime && age >= (period/1000))
			  {
			    Iout = 0;
			    time += dt/1000;
			  }
			
			if (age >= ((period/1000) - EPS) && time >= Downtime) // if the time is greater than the period
 			{
				step += step_size; // the steps increase
				age = 0;
				time = 0;
			}
			abs_step = abs(step);
		}
		if (abs_step > (abs_rEnd+abs_rStart)) 
		{
			cycle++;
			step = 0;
			interage = 0;
			time = 0;
		}
	}

	output(0) = (Iout*.5*1e-3); //mAmps. Output to amp is scaled 50mv = 100 pA
}

void MRT::update(DefaultGUIModel::update_flags_t flag) {
	switch (flag) {
		case INIT:
		        setParameter("Stim time (ms)", period);
			setParameter("Current Range Start (pA)", rStart);
			setParameter("Current Range End (pA)", rEnd);
			setParameter("Increment (pA)", step_size);
			setParameter("Cycles (#)", Ncycles);
			setParameter("Inter-Cycle-Interval (s)", ICI);
			setParameter("Downtime (s)", Downtime);
			break;

		case MODIFY:
			period = getParameter("Stim time (ms)").toDouble();
			rStart = getParameter("Current Range Start (pA)").toDouble();
			rEnd = getParameter("Current Range End (pA)").toDouble();
			step_size = getParameter("Increment (pA)").toDouble();
			Ncycles = getParameter("Cycles (#)").toInt();
			ICI = getParameter("Inter-Cycle-Interval (s)").toDouble();
			Downtime = getParameter("Downtime (s)").toDouble();
			break;

		case PAUSE:
			output(0) = 0;
	
		case PERIOD:
			dt = RT::System::getInstance()->getPeriod() * 1e-6;
	
		default:
			break;
	}
	
	// Some Error Checking for fun
	if (rStart > rEnd)
	{
		step_size = abs(step_size);
		step_size = -step_size;
	}
	if (rEnd < rStart)
	{
		step_size = abs(step_size);
	}
	if (period <= 0) {
		period = 0.25;
		setParameter("Period (sec)", period);
	}
	if (Ncycles < 1) {
		Ncycles = 1;
		setParameter("Cycles (#)", Ncycles);
	}
	//Initialize counters
	age = 0;
	step = 0;
	cycle = 0;
	interage = 0;	
}
