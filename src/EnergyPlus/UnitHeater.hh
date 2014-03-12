#ifndef UnitHeater_hh_INCLUDED
#define UnitHeater_hh_INCLUDED

// ObjexxFCL Headers
#include <ObjexxFCL/FArray1D.hh>
#include <ObjexxFCL/FArray1S.hh>
#include <ObjexxFCL/Fstring.hh>
#include <ObjexxFCL/Optional.hh>

// EnergyPlus Headers
#include <EnergyPlus.hh>
#include <DataGlobals.hh>

namespace EnergyPlus {

namespace UnitHeater {

	// Using/Aliasing
	using DataGlobals::MaxNameLength;

	// Data
	// MODULE PARAMETER DEFINITIONS
	extern Fstring const cMO_UnitHeater;

	// Character parameters for outside air control types:
	extern Fstring const ElectricCoil;
	extern Fstring const GasCoil;
	extern Fstring const WaterCoil;
	extern Fstring const SteamCoil;

	// DERIVED TYPE DEFINITIONS

	// MODULE VARIABLE DECLARATIONS:
	extern bool HCoilOn; // TRUE if the heating coil (gas or electric especially) should be running
	extern int NumOfUnitHeats; // Number of unit heaters in the input file
	extern Real64 QZnReq; // heating or cooling needed by zone [watts]
	extern FArray1D_bool MySizeFlag;
	extern FArray1D_bool CheckEquipName;

	// SUBROUTINE SPECIFICATIONS FOR MODULE UnitHeater

	// Types

	struct UnitHeaterData
	{
		// Members
		// Input data
		Fstring Name; // name of unit
		Fstring SchedName; // availability schedule
		int SchedPtr; // index to schedule
		int AirInNode; // inlet air node number
		int AirOutNode; // outlet air node number
		int FanType_Num; // Fan type number (see DataHVACGlobals)
		Fstring FanType; // type of fan
		Fstring FanName; // name of fan
		int Fan_Index;
		int FanSchedPtr; // index to fan operating mode schedule
		int FanAvailSchedPtr; // index to fan availability schedule
		int ControlCompTypeNum;
		int CompErrIndex;
		Real64 MaxAirVolFlow; // m3/s
		Real64 MaxAirMassFlow; // kg/s
		Fstring FanOperatesDuringNoHeating; // Indicates whether fan operates or not during no heating
		int FanOutletNode; // outlet node number for fan exit
		// (assumes fan is upstream of heating coil)
		int OpMode; // mode of operation; 1=cycling fan, cycling coil, 2=continuous fan, cycling coil
		Fstring HCoilType; // type of heating coil (water, gas, electric, etc.)
		Fstring HCoilTypeCh; // actual object name
		Fstring HCoilName; // name of heating coil
		int HCoil_Index;
		int HCoil_PlantTypeNum;
		int HCoil_FluidIndex;
		Real64 MaxVolHotWaterFlow; // m3/s
		Real64 MaxVolHotSteamFlow; // m3/s
		Real64 MaxHotWaterFlow; // kg/s
		Real64 MaxHotSteamFlow; // m3/s
		Real64 MinVolHotWaterFlow; // m3/s
		Real64 MinVolHotSteamFlow; // m3/s
		Real64 MinHotWaterFlow; // kg/s
		Real64 MinHotSteamFlow; // kg/s
		int HotControlNode; // hot water control node, inlet of coil
		Real64 HotControlOffset; // control tolerance
		int HotCoilOutNodeNum; // outlet of coil
		int HWLoopNum; // index for plant loop with hot plant coil
		int HWLoopSide; // index for plant loop side for hot plant coil
		int HWBranchNum; // index for plant branch for hot plant coil
		int HWCompNum; // index for plant component for hot plant coil
		Real64 PartLoadFrac; // part load fraction for the unit
		// Report data
		Real64 HeatPower; // unit heating output in watts
		Real64 HeatEnergy; // unit heating output in J
		Real64 ElecPower;
		Real64 ElecEnergy;
		Fstring AvailManagerListName; // Name of an availability manager list object
		int AvailStatus;
		bool FanOffNoHeating; // True when fan is on during no heating load
		Real64 FanPartLoadRatio; // fan part-load ratio for time step

		// Default Constructor
		UnitHeaterData() :
			Name( MaxNameLength ),
			SchedName( MaxNameLength ),
			SchedPtr( 0 ),
			AirInNode( 0 ),
			AirOutNode( 0 ),
			FanType_Num( 0 ),
			FanType( MaxNameLength ),
			FanName( MaxNameLength ),
			Fan_Index( 0 ),
			FanSchedPtr( 0 ),
			FanAvailSchedPtr( 0 ),
			ControlCompTypeNum( 0 ),
			CompErrIndex( 0 ),
			MaxAirVolFlow( 0.0 ),
			MaxAirMassFlow( 0.0 ),
			FanOperatesDuringNoHeating( MaxNameLength ),
			FanOutletNode( 0 ),
			OpMode( 0 ),
			HCoilType( MaxNameLength ),
			HCoilTypeCh( MaxNameLength ),
			HCoilName( MaxNameLength ),
			HCoil_Index( 0 ),
			HCoil_PlantTypeNum( 0 ),
			HCoil_FluidIndex( 0 ),
			MaxVolHotWaterFlow( 0.0 ),
			MaxVolHotSteamFlow( 0.0 ),
			MaxHotWaterFlow( 0.0 ),
			MaxHotSteamFlow( 0.0 ),
			MinVolHotWaterFlow( 0.0 ),
			MinVolHotSteamFlow( 0.0 ),
			MinHotWaterFlow( 0.0 ),
			MinHotSteamFlow( 0.0 ),
			HotControlNode( 0 ),
			HotControlOffset( 0.0 ),
			HotCoilOutNodeNum( 0 ),
			HWLoopNum( 0 ),
			HWLoopSide( 0 ),
			HWBranchNum( 0 ),
			HWCompNum( 0 ),
			PartLoadFrac( 0.0 ),
			HeatPower( 0.0 ),
			HeatEnergy( 0.0 ),
			ElecPower( 0.0 ),
			ElecEnergy( 0.0 ),
			AvailManagerListName( MaxNameLength ),
			AvailStatus( 0 ),
			FanOffNoHeating( false ),
			FanPartLoadRatio( 0.0 )
		{}

		// Member Constructor
		UnitHeaterData(
			Fstring const & Name, // name of unit
			Fstring const & SchedName, // availability schedule
			int const SchedPtr, // index to schedule
			int const AirInNode, // inlet air node number
			int const AirOutNode, // outlet air node number
			int const FanType_Num, // Fan type number (see DataHVACGlobals)
			Fstring const & FanType, // type of fan
			Fstring const & FanName, // name of fan
			int const Fan_Index,
			int const FanSchedPtr, // index to fan operating mode schedule
			int const FanAvailSchedPtr, // index to fan availability schedule
			int const ControlCompTypeNum,
			int const CompErrIndex,
			Real64 const MaxAirVolFlow, // m3/s
			Real64 const MaxAirMassFlow, // kg/s
			Fstring const & FanOperatesDuringNoHeating, // Indicates whether fan operates or not during no heating
			int const FanOutletNode, // outlet node number for fan exit
			int const OpMode, // mode of operation; 1=cycling fan, cycling coil, 2=continuous fan, cycling coil
			Fstring const & HCoilType, // type of heating coil (water, gas, electric, etc.)
			Fstring const & HCoilTypeCh, // actual object name
			Fstring const & HCoilName, // name of heating coil
			int const HCoil_Index,
			int const HCoil_PlantTypeNum,
			int const HCoil_FluidIndex,
			Real64 const MaxVolHotWaterFlow, // m3/s
			Real64 const MaxVolHotSteamFlow, // m3/s
			Real64 const MaxHotWaterFlow, // kg/s
			Real64 const MaxHotSteamFlow, // m3/s
			Real64 const MinVolHotWaterFlow, // m3/s
			Real64 const MinVolHotSteamFlow, // m3/s
			Real64 const MinHotWaterFlow, // kg/s
			Real64 const MinHotSteamFlow, // kg/s
			int const HotControlNode, // hot water control node, inlet of coil
			Real64 const HotControlOffset, // control tolerance
			int const HotCoilOutNodeNum, // outlet of coil
			int const HWLoopNum, // index for plant loop with hot plant coil
			int const HWLoopSide, // index for plant loop side for hot plant coil
			int const HWBranchNum, // index for plant branch for hot plant coil
			int const HWCompNum, // index for plant component for hot plant coil
			Real64 const PartLoadFrac, // part load fraction for the unit
			Real64 const HeatPower, // unit heating output in watts
			Real64 const HeatEnergy, // unit heating output in J
			Real64 const ElecPower,
			Real64 const ElecEnergy,
			Fstring const & AvailManagerListName, // Name of an availability manager list object
			int const AvailStatus,
			bool const FanOffNoHeating, // True when fan is on during no heating load
			Real64 const FanPartLoadRatio // fan part-load ratio for time step
		) :
			Name( MaxNameLength, Name ),
			SchedName( MaxNameLength, SchedName ),
			SchedPtr( SchedPtr ),
			AirInNode( AirInNode ),
			AirOutNode( AirOutNode ),
			FanType_Num( FanType_Num ),
			FanType( MaxNameLength, FanType ),
			FanName( MaxNameLength, FanName ),
			Fan_Index( Fan_Index ),
			FanSchedPtr( FanSchedPtr ),
			FanAvailSchedPtr( FanAvailSchedPtr ),
			ControlCompTypeNum( ControlCompTypeNum ),
			CompErrIndex( CompErrIndex ),
			MaxAirVolFlow( MaxAirVolFlow ),
			MaxAirMassFlow( MaxAirMassFlow ),
			FanOperatesDuringNoHeating( MaxNameLength, FanOperatesDuringNoHeating ),
			FanOutletNode( FanOutletNode ),
			OpMode( OpMode ),
			HCoilType( MaxNameLength, HCoilType ),
			HCoilTypeCh( MaxNameLength, HCoilTypeCh ),
			HCoilName( MaxNameLength, HCoilName ),
			HCoil_Index( HCoil_Index ),
			HCoil_PlantTypeNum( HCoil_PlantTypeNum ),
			HCoil_FluidIndex( HCoil_FluidIndex ),
			MaxVolHotWaterFlow( MaxVolHotWaterFlow ),
			MaxVolHotSteamFlow( MaxVolHotSteamFlow ),
			MaxHotWaterFlow( MaxHotWaterFlow ),
			MaxHotSteamFlow( MaxHotSteamFlow ),
			MinVolHotWaterFlow( MinVolHotWaterFlow ),
			MinVolHotSteamFlow( MinVolHotSteamFlow ),
			MinHotWaterFlow( MinHotWaterFlow ),
			MinHotSteamFlow( MinHotSteamFlow ),
			HotControlNode( HotControlNode ),
			HotControlOffset( HotControlOffset ),
			HotCoilOutNodeNum( HotCoilOutNodeNum ),
			HWLoopNum( HWLoopNum ),
			HWLoopSide( HWLoopSide ),
			HWBranchNum( HWBranchNum ),
			HWCompNum( HWCompNum ),
			PartLoadFrac( PartLoadFrac ),
			HeatPower( HeatPower ),
			HeatEnergy( HeatEnergy ),
			ElecPower( ElecPower ),
			ElecEnergy( ElecEnergy ),
			AvailManagerListName( MaxNameLength, AvailManagerListName ),
			AvailStatus( AvailStatus ),
			FanOffNoHeating( FanOffNoHeating ),
			FanPartLoadRatio( FanPartLoadRatio )
		{}

	};

	// Object Data
	extern FArray1D< UnitHeaterData > UnitHeat;

	// Functions

	void
	SimUnitHeater(
		Fstring const & CompName, // name of the fan coil unit
		int const ZoneNum, // number of zone being served
		bool const FirstHVACIteration, // TRUE if 1st HVAC simulation of system timestep
		Real64 & PowerMet, // Sensible power supplied (W)
		Real64 & LatOutputProvided, // Latent add/removal supplied by window AC (kg/s), dehumid = negative
		int & CompIndex
	);

	void
	GetUnitHeaterInput();

	void
	InitUnitHeater(
		int const UnitHeatNum, // index for the current unit heater
		int const ZoneNum, // number of zone being served
		bool const FirstHVACIteration // TRUE if 1st HVAC simulation of system timestep
	);

	void
	SizeUnitHeater( int const UnitHeatNum );

	void
	CalcUnitHeater(
		int & UnitHeatNum, // number of the current fan coil unit being simulated
		int const ZoneNum, // number of zone being served
		bool const FirstHVACIteration, // TRUE if 1st HVAC simulation of system timestep
		Real64 & PowerMet, // Sensible power supplied (W)
		Real64 & LatOutputProvided // Latent power supplied (kg/s), negative = dehumidification
	);

	void
	CalcUnitHeaterComponents(
		int const UnitHeatNum, // Unit index in unit heater array
		bool const FirstHVACIteration, // flag for 1st HVAV iteration in the time step
		Real64 & LoadMet, // load met by unit (watts)
		Optional_int_const OpMode = _, // fan operating mode
		Optional< Real64 const > PartLoadRatio = _ // part-load ratio
	);

	//SUBROUTINE UpdateUnitHeater

	// No update routine needed in this module since all of the updates happen on
	// the Node derived type directly and these updates are done by other routines.

	//END SUBROUTINE UpdateUnitHeater

	void
	ReportUnitHeater( int const UnitHeatNum ); // Unit index in unit heater array

	Real64
	CalcUnitHeaterResidual(
		Real64 const PartLoadRatio, // heating coil part load ratio
		Optional< FArray1S< Real64 > const > Par = _ // Function parameters
	);

	//     NOTICE

	//     Copyright � 1996-2014 The Board of Trustees of the University of Illinois
	//     and The Regents of the University of California through Ernest Orlando Lawrence
	//     Berkeley National Laboratory.  All rights reserved.

	//     Portions of the EnergyPlus software package have been developed and copyrighted
	//     by other individuals, companies and institutions.  These portions have been
	//     incorporated into the EnergyPlus software package under license.   For a complete
	//     list of contributors, see "Notice" located in EnergyPlus.f90.

	//     NOTICE: The U.S. Government is granted for itself and others acting on its
	//     behalf a paid-up, nonexclusive, irrevocable, worldwide license in this data to
	//     reproduce, prepare derivative works, and perform publicly and display publicly.
	//     Beginning five (5) years after permission to assert copyright is granted,
	//     subject to two possible five year renewals, the U.S. Government is granted for
	//     itself and others acting on its behalf a paid-up, non-exclusive, irrevocable
	//     worldwide license in this data to reproduce, prepare derivative works,
	//     distribute copies to the public, perform publicly and display publicly, and to
	//     permit others to do so.

	//     TRADEMARKS: EnergyPlus is a trademark of the US Department of Energy.

} // UnitHeater

} // EnergyPlus

#endif