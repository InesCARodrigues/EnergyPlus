#ifndef EvaporativeFluidCoolers_hh_INCLUDED
#define EvaporativeFluidCoolers_hh_INCLUDED

// ObjexxFCL Headers
#include <ObjexxFCL/FArray1D.hh>
#include <ObjexxFCL/FArray1S.hh>
#include <ObjexxFCL/Optional.hh>

// EnergyPlus Headers
#include <EnergyPlus.hh>
#include <DataGlobals.hh>

namespace EnergyPlus {

namespace EvaporativeFluidCoolers {

	// Using/Aliasing

	// Data
	// MODULE PARAMETER DEFINITIONS

	extern std::string const cEvapFluidCooler_SingleSpeed;
	extern std::string const cEvapFluidCooler_TwoSpeed;

	extern int const EvapLossByUserFactor;
	extern int const EvapLossByMoistTheory;

	extern int const BlowdownByConcentration;
	extern int const BlowdownBySchedule;

	extern int const PIM_StandardDesignCapacity;
	extern int const PIM_UFactor;
	extern int const PIM_UserSpecifiedDesignCapacity;

	extern int const EvapFluidCooler_SingleSpeed;
	extern int const EvapFluidCooler_TwoSpeed;

	// DERIVED TYPE DEFINITIONS

	// MODULE VARIABLE DECLARATIONS:
	extern int NumSimpleEvapFluidCoolers; // Number of similar evaporative fluid coolers

	// The following block of variables are used to carry model results for a evaporative fluid cooler instance
	//   across sim, update, and report routines.  Simulation manager must be careful
	//   in models with multiple evaporative fluid coolers.

	extern Real64 InletWaterTemp; // CW temperature at evaporative fluid cooler inlet
	extern Real64 OutletWaterTemp; // CW temperature at evaporative fluid cooler outlet
	extern int WaterInletNode; // Node number at evaporative fluid cooler inlet
	extern int WaterOutletNode; // Node number at evaporative fluid cooler outlet
	extern Real64 WaterMassFlowRate; // WaterMassFlowRate through evaporative fluid cooler
	//DSU this is plant level stuff now REAL(r64)   :: EvapFluidCoolerMassFlowRateMax     = 0.0d0    ! Max Hardware Mass Flow Rate
	//DSU this is plant level stuff now REAL(r64)   :: EvapFluidCoolerMassFlowRateMin     = 0.0d0    ! Min Hardware Mass Flow Rate
	//DSU this is plant level stuff now REAL(r64)   :: LoopMassFlowRateMaxAvail = 0.0d0    ! Max Loop Mass Flow Rate available
	//DSU this is plant level stuff now REAL(r64)   :: LoopMassFlowRateMinAvail = 0.0d0    ! Min Loop Mass Flow Rate available
	extern Real64 Qactual; // Evaporative fluid cooler heat transfer
	extern Real64 FanPower; // Evaporative fluid cooler fan power used
	extern Real64 AirFlowRateRatio; // Ratio of air flow rate through VS evaporative fluid cooler
	// to design air flow rate
	extern Real64 WaterUsage; // Evaporative fluid cooler water usage (m3/s)

	extern FArray1D_bool CheckEquipName;

	// SUBROUTINE SPECIFICATIONS FOR MODULE EvaporativeFluidCoolers

	// Driver/Manager Routines

	// Get Input routines for module

	// Initialization routines for module
	// also, calculates UA based on Standard Design Capacity input(s)

	// Update routines to check convergence and update nodes

	// Types

	struct EvapFluidCoolerspecs
	{
		// Members
		std::string Name; // User identifier
		std::string EvapFluidCoolerType; // Type of evaporative fluid cooler
		int EvapFluidCoolerType_Num;
		int PerformanceInputMethod_Num;
		bool Available; // need an array of logicals--load identifiers of available equipment
		bool ON; // Simulate the machine at it's operating part load ratio
		Real64 DesignWaterFlowRate; // Design water flow rate through the evaporative fluid cooler [m3/s]
		Real64 DesignSprayWaterFlowRate; // Design spray water flow rate through the evaporative fluid cooler [m3/s]
		Real64 DesWaterMassFlowRate; // Design water flow rate through the evaporative fluid cooler [kg/s]
		Real64 HighSpeedAirFlowRate; // Air flow rate through evaporative fluid cooler at high speed [m3/s]
		Real64 HighSpeedFanPower; // Fan power at high fan speed [W]
		Real64 HighSpeedEvapFluidCoolerUA; // UA of evaporative fluid cooler at high fan speed [W/C]
		Real64 LowSpeedAirFlowRate; // Air flow rate through evaporative fluid cooler at low speed [m3/s]
		Real64 LowSpeedAirFlowRateSizingFactor; // sizing factor for low speed air flow rate []
		Real64 LowSpeedFanPower; // Fan power at low fan speed [W]
		Real64 LowSpeedFanPowerSizingFactor; // Sizing factor for low speed fan power []
		Real64 LowSpeedEvapFluidCoolerUA; // UA of evaporative fluid cooler at low fan speed [W/C]
		Real64 LowSpeedEvapFluidCoolerUASizingFactor; // sizing factor for low speed UA []
		Real64 DesignEnteringWaterTemp; // Entering water temperature at design conditions
		Real64 DesignEnteringAirTemp; // Design inlet air dry-bulb temperature (C)
		Real64 DesignEnteringAirWetBulbTemp; // Design inlet air wet-bulb temperature (C)
		Real64 EvapFluidCoolerMassFlowRateMultiplier; // Maximum evaporative fluid cooler flow rate is
		// this multiplier times design flow rate
		Real64 HeatRejectCapNomCapSizingRatio; // ratio of actual cap to nominal capacity []
		Real64 HighSpeedStandardDesignCapacity; // Standard Design Capacity of the evaporative fluid cooler [W]
		// with entering water at 35C (95F),
		//  leaving water at 29.44C (85F), entering air at 25.56C (78F) wet-bulb
		//  temp and 35C (95F) dry-bulb temp, and water flow
		//  rate of 5.382E-8 m3/s per watt (3 gpm/ton)
		Real64 LowSpeedStandardDesignCapacity; // Standard Design Capacity of the evaporative fluid cooler [W]
		// with entering water at 35C (95F),
		//  leaving water at 29.44C (85F), entering air at 25.56C (78F) wet-bulb
		//  temp and 35C (95F) dry-bulb temp, and water flow
		//  rate of 5.382E-8 m3/s per watt (3 gpm/ton)
		Real64 LowSpeedStandardDesignCapacitySizingFactor; // sizing factor for low speed capacity []
		Real64 HighSpeedUserSpecifiedDesignCapacity; // User specified design capacity [W]
		Real64 LowSpeedUserSpecifiedDesignCapacity; // User specified design capacity for at low speed for
		// two speed fluid cooler[W]
		Real64 LowSpeedUserSpecifiedDesignCapacitySizingFactor; // sizing factor for low speed user capacity []
		Real64 Concentration; // fluid/glycol concentration - percent
		int FluidIndex; // Index to Property arrays
		Real64 SizFac; // sizing factor
		int WaterInletNodeNum; // Node number on the water inlet side of the evaporative fluid cooler
		int WaterOutletNodeNum; // Node number on the water outlet side of the evaporative fluid cooler
		int OutdoorAirInletNodeNum; // Node number of outdoor air inlet for the evaporative fluid cooler
		int BlowDownSchedulePtr; // Pointer to blow down schedule
		int HighMassFlowErrorCount; // Counter when mass flow rate is >
		// Design*EvapFluidCoolerMassFlowRateMultiplier
		int HighMassFlowErrorIndex; // Index for high mass flow recurring error message
		int OutletWaterTempErrorCount; // Counter when outlet water temperature is < minimum allowed temperature
		int OutletWaterTempErrorIndex; // Index for outlet water temperature recurring error message
		int SmallWaterMassFlowErrorCount; // Counter when water mass flow rate is very small
		int SmallWaterMassFlowErrorIndex; // Index for very small water mass flow rate recurring error message
		int WMFRLessThanMinAvailErrCount; // Counter when water mass flow rate is less than minimum available
		int WMFRLessThanMinAvailErrIndex; // Index for water mass flow rate less than minavail recurring message
		int WMFRGreaterThanMaxAvailErrCount; // Counter when water mass flow rate is greater than minimum available
		int WMFRGreaterThanMaxAvailErrIndex; // Index for water mass flow rate > minavail recurring message
		int EvapFluidCoolerAFRRFailedCount; // Counter for air flow rate ratio out of bounds error
		int EvapFluidCoolerAFRRFailedIndex; // Index for air flow rate ratio out of bounds error
		//fluid bypass
		int CapacityControl; // Type of capacity control for single speed cooling tower:
		//  0 - FanCycling, 1 - FluidBypass
		Real64 BypassFraction; // Fraction of fluid bypass as a ratio of total fluid flow
		//  through the tower sump
		//begin water system interactions
		int EvapLossMode; // sets how evaporative fluid cooler water evaporation is modeled
		int BlowdownMode; // sets how evaporative fluid cooler water blowdown is modeled
		int SchedIDBlowdown; // index "pointer" to schedule of blowdown in [m3/s]
		int WaterTankID; // index "pointer" to WaterStorage structure
		int WaterTankDemandARRID; // index "pointer" to demand array inside WaterStorage structure
		Real64 UserEvapLossFactor; // simple model [%/Delt C]
		Real64 DriftLossFraction;
		Real64 ConcentrationRatio; // ratio of solids in blowdown vs make up water
		bool SuppliedByWaterSystem;
		//end water system variables
		//loop topology variables
		int LoopNum;
		int LoopSideNum;
		int BranchNum;
		int CompNum;

		// Default Constructor
		EvapFluidCoolerspecs() :
			EvapFluidCoolerType_Num( 0 ),
			PerformanceInputMethod_Num( 0 ),
			Available( true ),
			ON( true ),
			DesignWaterFlowRate( 0.0 ),
			DesignSprayWaterFlowRate( 0.0 ),
			DesWaterMassFlowRate( 0.0 ),
			HighSpeedAirFlowRate( 0.0 ),
			HighSpeedFanPower( 0.0 ),
			HighSpeedEvapFluidCoolerUA( 0.0 ),
			LowSpeedAirFlowRate( 0.0 ),
			LowSpeedAirFlowRateSizingFactor( 0.0 ),
			LowSpeedFanPower( 0.0 ),
			LowSpeedFanPowerSizingFactor( 0.0 ),
			LowSpeedEvapFluidCoolerUA( 0.0 ),
			LowSpeedEvapFluidCoolerUASizingFactor( 0.0 ),
			DesignEnteringWaterTemp( 0.0 ),
			DesignEnteringAirTemp( 0.0 ),
			DesignEnteringAirWetBulbTemp( 0.0 ),
			EvapFluidCoolerMassFlowRateMultiplier( 0.0 ),
			HeatRejectCapNomCapSizingRatio( 0.0 ),
			HighSpeedStandardDesignCapacity( 0.0 ),
			LowSpeedStandardDesignCapacity( 0.0 ),
			LowSpeedStandardDesignCapacitySizingFactor( 0.0 ),
			HighSpeedUserSpecifiedDesignCapacity( 0.0 ),
			LowSpeedUserSpecifiedDesignCapacity( 0.0 ),
			LowSpeedUserSpecifiedDesignCapacitySizingFactor( 0.0 ),
			Concentration( 0.0 ),
			FluidIndex( 0 ),
			SizFac( 0.0 ),
			WaterInletNodeNum( 0 ),
			WaterOutletNodeNum( 0 ),
			OutdoorAirInletNodeNum( 0 ),
			BlowDownSchedulePtr( 0 ),
			HighMassFlowErrorCount( 0 ),
			HighMassFlowErrorIndex( 0 ),
			OutletWaterTempErrorCount( 0 ),
			OutletWaterTempErrorIndex( 0 ),
			SmallWaterMassFlowErrorCount( 0 ),
			SmallWaterMassFlowErrorIndex( 0 ),
			WMFRLessThanMinAvailErrCount( 0 ),
			WMFRLessThanMinAvailErrIndex( 0 ),
			WMFRGreaterThanMaxAvailErrCount( 0 ),
			WMFRGreaterThanMaxAvailErrIndex( 0 ),
			EvapFluidCoolerAFRRFailedCount( 0 ),
			EvapFluidCoolerAFRRFailedIndex( 0 ),
			CapacityControl( 0 ),
			BypassFraction( 0.0 ),
			EvapLossMode( EvapLossByMoistTheory ),
			BlowdownMode( BlowdownByConcentration ),
			SchedIDBlowdown( 0 ),
			WaterTankID( 0 ),
			WaterTankDemandARRID( 0 ),
			UserEvapLossFactor( 0.0 ),
			DriftLossFraction( 0.0 ),
			ConcentrationRatio( 0.0 ),
			SuppliedByWaterSystem( false ),
			LoopNum( 0 ),
			LoopSideNum( 0 ),
			BranchNum( 0 ),
			CompNum( 0 )
		{}

		// Member Constructor
		EvapFluidCoolerspecs(
			std::string const & Name, // User identifier
			std::string const & EvapFluidCoolerType, // Type of evaporative fluid cooler
			int const EvapFluidCoolerType_Num,
			int const PerformanceInputMethod_Num,
			bool const Available, // need an array of logicals--load identifiers of available equipment
			bool const ON, // Simulate the machine at it's operating part load ratio
			Real64 const DesignWaterFlowRate, // Design water flow rate through the evaporative fluid cooler [m3/s]
			Real64 const DesignSprayWaterFlowRate, // Design spray water flow rate through the evaporative fluid cooler [m3/s]
			Real64 const DesWaterMassFlowRate, // Design water flow rate through the evaporative fluid cooler [kg/s]
			Real64 const HighSpeedAirFlowRate, // Air flow rate through evaporative fluid cooler at high speed [m3/s]
			Real64 const HighSpeedFanPower, // Fan power at high fan speed [W]
			Real64 const HighSpeedEvapFluidCoolerUA, // UA of evaporative fluid cooler at high fan speed [W/C]
			Real64 const LowSpeedAirFlowRate, // Air flow rate through evaporative fluid cooler at low speed [m3/s]
			Real64 const LowSpeedAirFlowRateSizingFactor, // sizing factor for low speed air flow rate []
			Real64 const LowSpeedFanPower, // Fan power at low fan speed [W]
			Real64 const LowSpeedFanPowerSizingFactor, // Sizing factor for low speed fan power []
			Real64 const LowSpeedEvapFluidCoolerUA, // UA of evaporative fluid cooler at low fan speed [W/C]
			Real64 const LowSpeedEvapFluidCoolerUASizingFactor, // sizing factor for low speed UA []
			Real64 const DesignEnteringWaterTemp, // Entering water temperature at design conditions
			Real64 const DesignEnteringAirTemp, // Design inlet air dry-bulb temperature (C)
			Real64 const DesignEnteringAirWetBulbTemp, // Design inlet air wet-bulb temperature (C)
			Real64 const EvapFluidCoolerMassFlowRateMultiplier, // Maximum evaporative fluid cooler flow rate is
			Real64 const HeatRejectCapNomCapSizingRatio, // ratio of actual cap to nominal capacity []
			Real64 const HighSpeedStandardDesignCapacity, // Standard Design Capacity of the evaporative fluid cooler [W]
			Real64 const LowSpeedStandardDesignCapacity, // Standard Design Capacity of the evaporative fluid cooler [W]
			Real64 const LowSpeedStandardDesignCapacitySizingFactor, // sizing factor for low speed capacity []
			Real64 const HighSpeedUserSpecifiedDesignCapacity, // User specified design capacity [W]
			Real64 const LowSpeedUserSpecifiedDesignCapacity, // User specified design capacity for at low speed for
			Real64 const LowSpeedUserSpecifiedDesignCapacitySizingFactor, // sizing factor for low speed user capacity []
			Real64 const Concentration, // fluid/glycol concentration - percent
			int const FluidIndex, // Index to Property arrays
			Real64 const SizFac, // sizing factor
			int const WaterInletNodeNum, // Node number on the water inlet side of the evaporative fluid cooler
			int const WaterOutletNodeNum, // Node number on the water outlet side of the evaporative fluid cooler
			int const OutdoorAirInletNodeNum, // Node number of outdoor air inlet for the evaporative fluid cooler
			int const BlowDownSchedulePtr, // Pointer to blow down schedule
			int const HighMassFlowErrorCount, // Counter when mass flow rate is >
			int const HighMassFlowErrorIndex, // Index for high mass flow recurring error message
			int const OutletWaterTempErrorCount, // Counter when outlet water temperature is < minimum allowed temperature
			int const OutletWaterTempErrorIndex, // Index for outlet water temperature recurring error message
			int const SmallWaterMassFlowErrorCount, // Counter when water mass flow rate is very small
			int const SmallWaterMassFlowErrorIndex, // Index for very small water mass flow rate recurring error message
			int const WMFRLessThanMinAvailErrCount, // Counter when water mass flow rate is less than minimum available
			int const WMFRLessThanMinAvailErrIndex, // Index for water mass flow rate less than minavail recurring message
			int const WMFRGreaterThanMaxAvailErrCount, // Counter when water mass flow rate is greater than minimum available
			int const WMFRGreaterThanMaxAvailErrIndex, // Index for water mass flow rate > minavail recurring message
			int const EvapFluidCoolerAFRRFailedCount, // Counter for air flow rate ratio out of bounds error
			int const EvapFluidCoolerAFRRFailedIndex, // Index for air flow rate ratio out of bounds error
			int const CapacityControl, // Type of capacity control for single speed cooling tower:
			Real64 const BypassFraction, // Fraction of fluid bypass as a ratio of total fluid flow
			int const EvapLossMode, // sets how evaporative fluid cooler water evaporation is modeled
			int const BlowdownMode, // sets how evaporative fluid cooler water blowdown is modeled
			int const SchedIDBlowdown, // index "pointer" to schedule of blowdown in [m3/s]
			int const WaterTankID, // index "pointer" to WaterStorage structure
			int const WaterTankDemandARRID, // index "pointer" to demand array inside WaterStorage structure
			Real64 const UserEvapLossFactor, // simple model [%/Delt C]
			Real64 const DriftLossFraction,
			Real64 const ConcentrationRatio, // ratio of solids in blowdown vs make up water
			bool const SuppliedByWaterSystem,
			int const LoopNum,
			int const LoopSideNum,
			int const BranchNum,
			int const CompNum
		) :
			Name( Name ),
			EvapFluidCoolerType( EvapFluidCoolerType ),
			EvapFluidCoolerType_Num( EvapFluidCoolerType_Num ),
			PerformanceInputMethod_Num( PerformanceInputMethod_Num ),
			Available( Available ),
			ON( ON ),
			DesignWaterFlowRate( DesignWaterFlowRate ),
			DesignSprayWaterFlowRate( DesignSprayWaterFlowRate ),
			DesWaterMassFlowRate( DesWaterMassFlowRate ),
			HighSpeedAirFlowRate( HighSpeedAirFlowRate ),
			HighSpeedFanPower( HighSpeedFanPower ),
			HighSpeedEvapFluidCoolerUA( HighSpeedEvapFluidCoolerUA ),
			LowSpeedAirFlowRate( LowSpeedAirFlowRate ),
			LowSpeedAirFlowRateSizingFactor( LowSpeedAirFlowRateSizingFactor ),
			LowSpeedFanPower( LowSpeedFanPower ),
			LowSpeedFanPowerSizingFactor( LowSpeedFanPowerSizingFactor ),
			LowSpeedEvapFluidCoolerUA( LowSpeedEvapFluidCoolerUA ),
			LowSpeedEvapFluidCoolerUASizingFactor( LowSpeedEvapFluidCoolerUASizingFactor ),
			DesignEnteringWaterTemp( DesignEnteringWaterTemp ),
			DesignEnteringAirTemp( DesignEnteringAirTemp ),
			DesignEnteringAirWetBulbTemp( DesignEnteringAirWetBulbTemp ),
			EvapFluidCoolerMassFlowRateMultiplier( EvapFluidCoolerMassFlowRateMultiplier ),
			HeatRejectCapNomCapSizingRatio( HeatRejectCapNomCapSizingRatio ),
			HighSpeedStandardDesignCapacity( HighSpeedStandardDesignCapacity ),
			LowSpeedStandardDesignCapacity( LowSpeedStandardDesignCapacity ),
			LowSpeedStandardDesignCapacitySizingFactor( LowSpeedStandardDesignCapacitySizingFactor ),
			HighSpeedUserSpecifiedDesignCapacity( HighSpeedUserSpecifiedDesignCapacity ),
			LowSpeedUserSpecifiedDesignCapacity( LowSpeedUserSpecifiedDesignCapacity ),
			LowSpeedUserSpecifiedDesignCapacitySizingFactor( LowSpeedUserSpecifiedDesignCapacitySizingFactor ),
			Concentration( Concentration ),
			FluidIndex( FluidIndex ),
			SizFac( SizFac ),
			WaterInletNodeNum( WaterInletNodeNum ),
			WaterOutletNodeNum( WaterOutletNodeNum ),
			OutdoorAirInletNodeNum( OutdoorAirInletNodeNum ),
			BlowDownSchedulePtr( BlowDownSchedulePtr ),
			HighMassFlowErrorCount( HighMassFlowErrorCount ),
			HighMassFlowErrorIndex( HighMassFlowErrorIndex ),
			OutletWaterTempErrorCount( OutletWaterTempErrorCount ),
			OutletWaterTempErrorIndex( OutletWaterTempErrorIndex ),
			SmallWaterMassFlowErrorCount( SmallWaterMassFlowErrorCount ),
			SmallWaterMassFlowErrorIndex( SmallWaterMassFlowErrorIndex ),
			WMFRLessThanMinAvailErrCount( WMFRLessThanMinAvailErrCount ),
			WMFRLessThanMinAvailErrIndex( WMFRLessThanMinAvailErrIndex ),
			WMFRGreaterThanMaxAvailErrCount( WMFRGreaterThanMaxAvailErrCount ),
			WMFRGreaterThanMaxAvailErrIndex( WMFRGreaterThanMaxAvailErrIndex ),
			EvapFluidCoolerAFRRFailedCount( EvapFluidCoolerAFRRFailedCount ),
			EvapFluidCoolerAFRRFailedIndex( EvapFluidCoolerAFRRFailedIndex ),
			CapacityControl( CapacityControl ),
			BypassFraction( BypassFraction ),
			EvapLossMode( EvapLossMode ),
			BlowdownMode( BlowdownMode ),
			SchedIDBlowdown( SchedIDBlowdown ),
			WaterTankID( WaterTankID ),
			WaterTankDemandARRID( WaterTankDemandARRID ),
			UserEvapLossFactor( UserEvapLossFactor ),
			DriftLossFraction( DriftLossFraction ),
			ConcentrationRatio( ConcentrationRatio ),
			SuppliedByWaterSystem( SuppliedByWaterSystem ),
			LoopNum( LoopNum ),
			LoopSideNum( LoopSideNum ),
			BranchNum( BranchNum ),
			CompNum( CompNum )
		{}

	};

	struct EvapFluidCoolerInletConds
	{
		// Members
		Real64 WaterTemp; // Evaporative fluid cooler water inlet temperature (C)
		Real64 AirTemp; // Evaporative fluid cooler air inlet dry-bulb temperature (C)
		Real64 AirWetBulb; // Evaporative fluid cooler air inlet wet-bulb temperature (C)
		Real64 AirPress; // Evaporative fluid cooler air barometric pressure
		Real64 AirHumRat; // Evaporative fluid cooler air inlet humidity ratio (kg/kg)

		// Default Constructor
		EvapFluidCoolerInletConds() :
			WaterTemp( 0.0 ),
			AirTemp( 0.0 ),
			AirWetBulb( 0.0 ),
			AirPress( 0.0 ),
			AirHumRat( 0.0 )
		{}

		// Member Constructor
		EvapFluidCoolerInletConds(
			Real64 const WaterTemp, // Evaporative fluid cooler water inlet temperature (C)
			Real64 const AirTemp, // Evaporative fluid cooler air inlet dry-bulb temperature (C)
			Real64 const AirWetBulb, // Evaporative fluid cooler air inlet wet-bulb temperature (C)
			Real64 const AirPress, // Evaporative fluid cooler air barometric pressure
			Real64 const AirHumRat // Evaporative fluid cooler air inlet humidity ratio (kg/kg)
		) :
			WaterTemp( WaterTemp ),
			AirTemp( AirTemp ),
			AirWetBulb( AirWetBulb ),
			AirPress( AirPress ),
			AirHumRat( AirHumRat )
		{}

	};

	struct ReportVars
	{
		// Members
		Real64 InletWaterTemp; // Evaporative fluid cooler inlet water temperature (C)
		Real64 OutletWaterTemp; // Evaporative fluid cooler outlet water temperature (C)
		Real64 WaterMassFlowRate; // Evaporative fluid cooler water mass flow rate (m3/s)
		Real64 Qactual; // Evaporative fluid cooler heat rejection rate (W)
		Real64 FanPower; // Evaporative fluid cooler fan power (W)
		Real64 FanEnergy; // Evaporative fluid cooler fan energy consumption (J)
		Real64 AirFlowRatio; // Air flow ratio through variable speed evaporative fluid cooler
		Real64 WaterAmountUsed; // Evaporative fluid cooler make up water usage (m3)
		Real64 EvaporationVdot;
		Real64 EvaporationVol;
		Real64 DriftVdot;
		Real64 DriftVol;
		Real64 BlowdownVdot;
		Real64 BlowdownVol;
		Real64 MakeUpVdot;
		Real64 MakeUpVol;
		Real64 TankSupplyVdot;
		Real64 TankSupplyVol;
		Real64 StarvedMakeUpVdot;
		Real64 StarvedMakeUpVol;
		Real64 BypassFraction; // Added for fluid bypass

		// Default Constructor
		ReportVars() :
			InletWaterTemp( 0.0 ),
			OutletWaterTemp( 0.0 ),
			WaterMassFlowRate( 0.0 ),
			Qactual( 0.0 ),
			FanPower( 0.0 ),
			FanEnergy( 0.0 ),
			AirFlowRatio( 0.0 ),
			WaterAmountUsed( 0.0 ),
			EvaporationVdot( 0.0 ),
			EvaporationVol( 0.0 ),
			DriftVdot( 0.0 ),
			DriftVol( 0.0 ),
			BlowdownVdot( 0.0 ),
			BlowdownVol( 0.0 ),
			MakeUpVdot( 0.0 ),
			MakeUpVol( 0.0 ),
			TankSupplyVdot( 0.0 ),
			TankSupplyVol( 0.0 ),
			StarvedMakeUpVdot( 0.0 ),
			StarvedMakeUpVol( 0.0 ),
			BypassFraction( 0.0 )
		{}

		// Member Constructor
		ReportVars(
			Real64 const InletWaterTemp, // Evaporative fluid cooler inlet water temperature (C)
			Real64 const OutletWaterTemp, // Evaporative fluid cooler outlet water temperature (C)
			Real64 const WaterMassFlowRate, // Evaporative fluid cooler water mass flow rate (m3/s)
			Real64 const Qactual, // Evaporative fluid cooler heat rejection rate (W)
			Real64 const FanPower, // Evaporative fluid cooler fan power (W)
			Real64 const FanEnergy, // Evaporative fluid cooler fan energy consumption (J)
			Real64 const AirFlowRatio, // Air flow ratio through variable speed evaporative fluid cooler
			Real64 const WaterAmountUsed, // Evaporative fluid cooler make up water usage (m3)
			Real64 const EvaporationVdot,
			Real64 const EvaporationVol,
			Real64 const DriftVdot,
			Real64 const DriftVol,
			Real64 const BlowdownVdot,
			Real64 const BlowdownVol,
			Real64 const MakeUpVdot,
			Real64 const MakeUpVol,
			Real64 const TankSupplyVdot,
			Real64 const TankSupplyVol,
			Real64 const StarvedMakeUpVdot,
			Real64 const StarvedMakeUpVol,
			Real64 const BypassFraction // Added for fluid bypass
		) :
			InletWaterTemp( InletWaterTemp ),
			OutletWaterTemp( OutletWaterTemp ),
			WaterMassFlowRate( WaterMassFlowRate ),
			Qactual( Qactual ),
			FanPower( FanPower ),
			FanEnergy( FanEnergy ),
			AirFlowRatio( AirFlowRatio ),
			WaterAmountUsed( WaterAmountUsed ),
			EvaporationVdot( EvaporationVdot ),
			EvaporationVol( EvaporationVol ),
			DriftVdot( DriftVdot ),
			DriftVol( DriftVol ),
			BlowdownVdot( BlowdownVdot ),
			BlowdownVol( BlowdownVol ),
			MakeUpVdot( MakeUpVdot ),
			MakeUpVol( MakeUpVol ),
			TankSupplyVdot( TankSupplyVdot ),
			TankSupplyVol( TankSupplyVol ),
			StarvedMakeUpVdot( StarvedMakeUpVdot ),
			StarvedMakeUpVol( StarvedMakeUpVol ),
			BypassFraction( BypassFraction )
		{}

	};

	// Object Data
	extern FArray1D< EvapFluidCoolerspecs > SimpleEvapFluidCooler; // dimension to number of machines
	extern FArray1D< EvapFluidCoolerInletConds > SimpleEvapFluidCoolerInlet; // inlet conditions
	extern FArray1D< ReportVars > SimpleEvapFluidCoolerReport; // report variables

	// Functions

	void
	SimEvapFluidCoolers(
		std::string const & EvapFluidCoolerType,
		std::string const & EvapFluidCoolerName,
		int & CompIndex,
		bool & RunFlag,
		bool const InitLoopEquip,
		Real64 & MaxCap,
		Real64 & MinCap,
		Real64 & OptCap,
		bool const GetSizingFactor, // TRUE when just the sizing factor is requested
		Real64 & SizingFactor // sizing factor
	);

	// End EvaporativeFluidCoolers Module Driver Subroutines
	//******************************************************************************

	// Beginning of EvaporativeFluidCoolers Module Get Input subroutines
	//******************************************************************************

	void
	GetEvapFluidCoolerInput();

	// End of Get Input subroutines for the Evaporative Fluid Cooler Module
	//******************************************************************************

	// Beginning Initialization Section for the Evaporative Fluid Coolers Module
	//******************************************************************************

	void
	InitSimVars();

	void
	InitEvapFluidCooler(
		int const EvapFluidCoolerNum, // Number of the current evaporative fluid cooler being simulated
		bool const RunFlag // Indication of
	);

	void
	SizeEvapFluidCooler( int const EvapFluidCoolerNum );

	// End Initialization Section for the EvaporativeFluidCoolers Module
	//******************************************************************************

	// Beginning of the EvaporativeFluidCoolers Module Simulation Subroutines
	// *****************************************************************************

	void
	CalcSingleSpeedEvapFluidCooler( int & EvapFluidCoolerNum );

	void
	CalcTwoSpeedEvapFluidCooler( int & EvapFluidCoolerNum );

	void
	SimSimpleEvapFluidCooler(
		int const EvapFluidCoolerNum,
		Real64 const WaterMassFlowRate,
		Real64 const AirFlowRate,
		Real64 const UAdesign,
		Real64 & OutletWaterTemp
	);

	Real64
	SimpleEvapFluidCoolerUAResidual(
		Real64 const UA, // UA of evaporative fluid cooler
		Optional< FArray1S< Real64 > const > Par = _ // par(1) = design evaporative fluid cooler load [W]
	);

	// End of the EvaporativeFluidCoolers Module Simulation Subroutines
	// *****************************************************************************

	void
	CalculateWaterUseage( int const EvapFluidCoolerNum );

	// Beginning of Record Keeping subroutines for the EvaporativeFluidCooler Module
	// *****************************************************************************

	void
	UpdateEvapFluidCooler( int const EvapFluidCoolerNum );

	// End of Record Keeping subroutines for the EvaporativeFluidCooler Module
	// *****************************************************************************

	// Beginning of Reporting subroutines for the EvaporativeFluidCooler Module
	// *****************************************************************************

	void
	ReportEvapFluidCooler(
		bool const RunFlag,
		int const EvapFluidCoolerNum
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

} // EvaporativeFluidCoolers

} // EnergyPlus

#endif
