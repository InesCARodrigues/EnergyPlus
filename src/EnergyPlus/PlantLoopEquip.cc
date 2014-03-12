// ObjexxFCL Headers
#include <ObjexxFCL/Fstring.hh>

// EnergyPlus Headers
#include <PlantLoopEquip.hh>
#include <BaseboardRadiator.hh>
#include <Boilers.hh>
#include <BoilerSteam.hh>
#include <ChillerAbsorption.hh>
#include <ChillerElectricEIR.hh>
#include <ChillerExhaustAbsorption.hh>
#include <ChillerGasAbsorption.hh>
#include <ChillerIndirectAbsorption.hh>
#include <ChillerReformulatedEIR.hh>
#include <CondenserLoopTowers.hh>
#include <CTElectricGenerator.hh>
#include <DataGlobals.hh>
#include <DataLoopNode.hh>
#include <DataPlant.hh>
#include <DataPrecisionGlobals.hh>
#include <EvaporativeFluidCoolers.hh>
#include <FluidCoolers.hh>
#include <FuelCellElectricGenerator.hh>
#include <GroundHeatExchangers.hh>
#include <HeatPumpWaterToWaterCOOLING.hh>
#include <HeatPumpWaterToWaterHEATING.hh>
#include <HeatPumpWaterToWaterSimple.hh>
#include <HVACVariableRefrigerantFlow.hh>
#include <HWBaseboardRadiator.hh>
#include <ICEngineElectricGenerator.hh>
#include <IceThermalStorage.hh>
#include <MicroCHPElectricGenerator.hh>
#include <MicroturbineElectricGenerator.hh>
#include <OutsideEnergySources.hh>
#include <PhotovoltaicThermalCollectors.hh>
#include <PipeHeatTransfer.hh>
#include <Pipes.hh>
#include <PlantCentralGSHP.hh>
#include <PlantChillers.hh>
#include <PlantComponentTemperatureSources.hh>
#include <PlantHeatExchangerFluidToFluid.hh>
#include <PlantLoadProfile.hh>
#include <PlantPipingSystemsManager.hh>
#include <PlantValves.hh>
#include <PondGroundHeatExchanger.hh>
#include <Pumps.hh>
#include <RefrigeratedCase.hh>
#include <ScheduleManager.hh>
#include <SolarCollectors.hh>
#include <SteamBaseboardRadiator.hh>
#include <SurfaceGroundHeatExchanger.hh>
#include <UserDefinedComponents.hh>
#include <UtilityRoutines.hh>
#include <WaterCoils.hh>
#include <WaterThermalTanks.hh>
#include <WaterUse.hh>

namespace EnergyPlus {

namespace PlantLoopEquip {

	// MODULE INFORMATION:
	//       AUTHOR         Sankaranarayanan K P
	//       DATE WRITTEN   July 2005
	//       MODIFIED       na
	//       RE-ENGINEERED  na

	// PURPOSE OF THIS MODULE:
	// This module contains subroutine that calls the required component for simulation. The components are selected
	// using a CASE statement.

	// METHODOLOGY EMPLOYED:
	// Needs description, as appropriate.

	// REFERENCES: none

	// OTHER NOTES: none

	// Using/Aliasing
	using namespace DataPrecisionGlobals;
	using DataGlobals::MaxNameLength;
	using namespace DataPlant;
	using DataLoopNode::Node;

	// Data
	// SUBROUTINE SPECIFICATION

	// MODULE SUBROUTINES

	// Functions

	void
	SimPlantEquip(
		int const LoopNum, // loop counter
		int const LoopSideNum, // loop counter
		int const BranchNum,
		int const Num,
		bool const FirstHVACIteration, // TRUE if First iteration of simulation
		bool & InitLoopEquip,
		bool const GetCompSizFac // Tells component routine to return the component sizing fraction
	)
	{

		// SUBROUTINE INFORMATION:
		//       AUTHOR         Dan Fisher
		//       DATE WRITTEN   July 1998
		//       MODIFIED       June 2000  -Brandon Anderson
		//                             Changed to Group Similar Components.  Components will
		//                         be defined by ComponentType:SpecificComponent.
		//                         The colon will act as the type delimeter, So all
		//                         components of one type will be grouped. ex.(Boilers,Chillers)
		//                       May 2003 - Simon Rees
		//                         Added initial loop to force free cooling chiller etc to be
		//                         simulated before other components.
		//       RE-ENGINEERED  na

		// PURPOSE OF THIS SUBROUTINE:
		// This subroutine calls the appropriate routines to simulate
		// the equipment on the plant.

		// METHODOLOGY EMPLOYED:
		// This subroutine employs a rule-based
		// scheme to operate the plant equipment simulation without
		// requiring a detailed flow network solver.  The scheme is based
		// on several restrictive assumptions which may be relaxed when
		// a more detailed solution technique is developed.  The current
		// assumptions are:
		//    1.   All loop cooling/heating equipment is connected
		//         in parallel.
		//    2.   Only one circulation pump may be specified per loop.
		//    3.   The circulation pump must be specified first in the
		//         simulation order and is assumed to be connected in
		//         series with the cooling/heating equipment.
		//    4.   The Circ. pump determines the maximum flow rate for
		//         the loop.
		// The scheme is valid only for Part Load based plant equipment
		// models (currently the only type implemented).  Each equipment
		// simulation updates its outlet node temperature, estimates its
		// flow rate and returns a remaining loop demand which is passed
		// on to the other available equipment.

		// NOTE: All Equipment return the index of their lists during "InitLoopEquip"
		// as a time reduction measure.  Specific ifs are set to catch those modules that don't.
		// If you add a module or new equipment type, you must set up this structure.

		// REFERENCES:
		// na

		// Using/Aliasing
		using ScheduleManager::GetCurrentScheduleValue;
		using Boilers::SimBoiler;
		using WaterThermalTanks::SimWaterThermalTank;
		using ChillerAbsorption::SimBLASTAbsorber;
		using ChillerIndirectAbsorption::SimIndirectAbsorber;
		using ChillerGasAbsorption::SimGasAbsorber;
		using ChillerExhaustAbsorption::SimExhaustAbsorber;
		using PlantChillers::SimChiller;
		using ChillerElectricEIR::SimElectricEIRChiller;
		using ChillerReformulatedEIR::SimReformulatedEIRChiller;
		using HeatPumpWaterToWaterHEATING::SimHPWatertoWaterHEATING;
		using HeatPumpWaterToWaterCOOLING::SimHPWatertoWaterCOOLING;
		using HeatPumpWaterToWaterSimple::SimHPWatertoWaterSimple;
		using OutsideEnergySources::SimOutsideEnergy;
		using Pipes::SimPipes;
		using PipeHeatTransfer::SimPipesHeatTransfer;
		using Pumps::SimPumps;

		using PlantHeatExchangerFluidToFluid::SimFluidHeatExchanger;
		using CondenserLoopTowers::SimTowers;
		using FluidCoolers::SimFluidCoolers;
		using EvaporativeFluidCoolers::SimEvapFluidCoolers;
		using BoilerSteam::SimSteamBoiler;
		using IceThermalStorage::SimIceStorage;
		using FuelCellElectricGenerator::SimFuelCellPlantHeatRecovery;
		using MicroCHPElectricGenerator::SimMicroCHPPlantHeatRecovery;
		using PlantValves::SimPlantValves;
		using ICEngineElectricGenerator::SimICEPlantHeatRecovery;
		using CTElectricGenerator::SimCTPlantHeatRecovery;
		using MicroturbineElectricGenerator::SimMTPlantHeatRecovery;
		using GroundHeatExchangers::SimGroundHeatExchangers;
		using SurfaceGroundHeatExchanger::SimSurfaceGroundHeatExchanger;
		using PondGroundHeatExchanger::SimPondGroundHeatExchanger;

		using PlantLoadProfile::SimulatePlantProfile;
		using WaterCoils::UpdateWaterToAirCoilPlantConnection;
		using WaterUse::SimulateWaterUseConnection;
		using SolarCollectors::SimSolarCollector;
		using BaseboardRadiator::UpdateBaseboardPlantConnection;
		using HWBaseboardRadiator::UpdateHWBaseboardPlantConnection;
		using SteamBaseboardRadiator::UpdateSteamBaseboardPlantConnection;
		using RefrigeratedCase::SimRefrigCondenser;
		using PhotovoltaicThermalCollectors::SimPVTcollectors;
		using PhotovoltaicThermalCollectors::CalledFromPlantLoopEquipMgr;
		using PlantPipingSystemsManager::SimPipingSystemCircuit;
		using UserDefinedComponents::SimUserDefinedPlantComponent;
		using HVACVariableRefrigerantFlow::SimVRFCondenserPlant;
		using PlantComponentTemperatureSources::SimWaterSource;
		using PlantCentralGSHP::SimCentralGroundSourceHeatPump;

		// Locals
		// SUBROUTINE ARGUMENT DEFINITIONS:

		// SUBROUTINE PARAMETER DEFINITIONS:
		// na

		// INTERFACE BLOCK SPECIFICATIONS
		// na

		// DERIVED TYPE DEFINITIONS
		// na

		// SUBROUTINE LOCAL VARIABLE DECLARATIONS:
		int CompNum; // Plant side component list equipment number
		int BrnNum; // Branch counter
		int EquipNum; // Plant side component list equipment number
		int EquipTypeNum;
		int BranchInletNode;
		int LastNodeOnBranch;
		int PumpOutletNode;
		int LoopControl;
		bool RunFlag; // TRUE if operating this iteration
		Fstring EquipType( MaxNameLength ); // local equipment type
		Fstring EquipName( MaxNameLength ); // local equipment name
		int EquipFlowCtrl;
		Real64 CurLoad;
		Real64 MaxLoad;
		Real64 MinLoad;
		Real64 OptLoad;
		Real64 SizingFac; // the component sizing fraction
		static Real64 BranchFlowRequest( 0.0 );
		static Real64 InitialBranchFlow( 0.0 );
		int GeneralEquipType; // Basic Equipment type from EquipType Used to help organize this routine
		static bool PumpPowerToLoop( false );
		static bool RunLoopPumps( false );
		Real64 TempCondInDesign; // Design condenser inlet temp. C , or 25.d0
		Real64 TempEvapOutDesign;

		// Based on the general equip type and the GetCompSizFac value, see if we can just leave early
		GeneralEquipType = PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).GeneralEquipType;
		if ( GetCompSizFac && ( GeneralEquipType != GenEquipTypes_Chiller && GeneralEquipType != GenEquipTypes_Boiler ) && GeneralEquipType != GenEquipTypes_CoolingTower ) {
			PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = 0.0;
			return;
		}

		//set local variables
		EquipType = PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TypeOf;
		EquipTypeNum = PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TypeOf_Num;
		EquipFlowCtrl = PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).FlowCtrl;
		GeneralEquipType = PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).GeneralEquipType;
		EquipName = PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).Name;
		EquipNum = PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum;
		RunFlag = PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).ON;
		CurLoad = PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MyLoad;

		//select equipment and call equiment simulation
		TypeOfEquip: { auto const SELECT_CASE_var( GeneralEquipType );
		//PIPES
		//Pipe has no special types at the moment, so find it this way
		if ( SELECT_CASE_var == GenEquipTypes_Pipe ) {
			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_Pipe ) {
				SimPipes( TypeOf_Pipe, EquipName, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).MaxVolFlowRate, InitLoopEquip, FirstHVACIteration );

			} else if ( SELECT_CASE_var1 == TypeOf_PipeSteam ) {
				SimPipes( TypeOf_PipeSteam, EquipName, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).MaxVolFlowRate, InitLoopEquip, FirstHVACIteration );

			} else if ( SELECT_CASE_var1 == TypeOf_PipeExterior ) {
				SimPipesHeatTransfer( TypeOf_PipeExterior, EquipName, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum, InitLoopEquip, FirstHVACIteration );

			} else if ( SELECT_CASE_var1 == TypeOf_PipeInterior ) {
				SimPipesHeatTransfer( TypeOf_PipeInterior, EquipName, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum, InitLoopEquip, FirstHVACIteration );

			} else if ( SELECT_CASE_var1 == TypeOf_PipeUnderground ) {
				SimPipesHeatTransfer( TypeOf_PipeUnderground, EquipName, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum, InitLoopEquip, FirstHVACIteration );

			} else if ( SELECT_CASE_var1 == TypeOf_PipingSystemPipeCircuit ) {
				SimPipingSystemCircuit( EquipName, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum, InitLoopEquip, FirstHVACIteration );

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Pipe Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

		} else if ( SELECT_CASE_var == GenEquipTypes_Pump ) {
			//DSU? This is still called by the sizing routine, is that OK?

			//      SELECT CASE(EquipTypeNum)
			//       CASE (TypeOf_LoopPump)
			//         ! Loop pumps are simulated before this routine
			//         PumpHeat = 0.0
			//       CASE (TypeOf_BranchPump)  ! This is the branch pump case
			//        BranchInletNode = PlantLoop(LoopNum)%LoopSide(LoopSideNum)%Branch(BranchNum)%NodeNumIn
			//        LastNodeOnBranch  = PlantLoop(LoopNum)%LoopSide(LoopSideNum)%Branch(BranchNum)%NodeNumOut
			//        BranchFlowRequest = Node(LastNodeOnBranch)%MassFlowRate
			//        IF(PlantLoop(LoopNum)%LoopSide(LoopSideNum)%FlowLock == 0) THEN    !DSU
			//          PumpPowerToLoop = .FALSE.
			//        ELSE
			//          PumpPowerToLoop = .TRUE.
			//        END IF
			//        CALL SimPumps(EquipName,LoopNum,BranchInletNode,BranchFlowRequest, &
			//                       RunLoopPumps,InitialBranchFlow,PumpPowerToLoop,     &
			//                       PlantLoop(LoopNum)%LoopSide(LoopSideNum)%Branch(BranchNum)%PumpIndex,PumpOutletNode,PumpHeat)
			//        PlantLoop(LoopNum)%LoopSide(LoopSideNum)%Branch(BranchNum)%Comp(Num)%ON = RunLoopPumps
			//        PlantLoop(LoopNum)%LoopSide(LoopSideNum)%Branch(BranchNum)%PumpMassFlow = node(PumpOutletNode)%MassFlowRate
			//        ! This will only stay TRUE for this branch
			//        If(PumpPowerToLoop) AddPumpHeat = .True.
			//        IF(PlantLoop(LoopNum)%LoopSide(SupplySide)%FlowLock == 0) Node(PumpOutletNode)%temp = &   !DSU
			//           PlantLoop(LoopNum)%LoopSide(DemandSide)%FlowRequestTemperature
			//    END SELECT

			//CHILLERS
		} else if ( SELECT_CASE_var == GenEquipTypes_Chiller ) {
			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( ( SELECT_CASE_var1 == TypeOf_Chiller_EngineDriven ) || ( SELECT_CASE_var1 == TypeOf_Chiller_Electric ) || ( SELECT_CASE_var1 == TypeOf_Chiller_ConstCOP ) || ( SELECT_CASE_var1 == TypeOf_Chiller_CombTurbine ) ) {
				SimChiller( LoopNum, LoopSideNum, EquipTypeNum, EquipName, EquipFlowCtrl, EquipNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac, TempCondInDesign, TempEvapOutDesign );
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TempDesCondIn = TempCondInDesign;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TempDesEvapOut = TempEvapOutDesign;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_Chiller_Absorption ) {
				SimBLASTAbsorber( EquipType, EquipName, EquipFlowCtrl, LoopNum, LoopSideNum, EquipNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac, TempCondInDesign );
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TempDesCondIn = TempCondInDesign;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_Chiller_Indirect_Absorption ) {
				SimIndirectAbsorber( EquipType, EquipName, EquipFlowCtrl, LoopNum, LoopSideNum, EquipNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac, TempCondInDesign );
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TempDesCondIn = TempCondInDesign;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_Chiller_ElectricEIR ) {
				SimElectricEIRChiller( EquipType, EquipName, EquipFlowCtrl, EquipNum, LoopNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac, TempCondInDesign, TempEvapOutDesign );
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TempDesCondIn = TempCondInDesign;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TempDesEvapOut = TempEvapOutDesign;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_Chiller_ElectricReformEIR ) {
				SimReformulatedEIRChiller( EquipType, EquipName, EquipFlowCtrl, EquipNum, LoopNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac, TempCondInDesign, TempEvapOutDesign );
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TempDesCondIn = TempCondInDesign;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TempDesEvapOut = TempEvapOutDesign;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

				// Chiller-Heater needs to know whether it is being called for heating or cooling
				// Since loops are generic, pass the branch inlet nodenum
			} else if ( SELECT_CASE_var1 == TypeOf_Chiller_DFAbsorption ) {
				SimGasAbsorber( EquipType, EquipName, EquipFlowCtrl, EquipNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).NodeNumIn, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac, TempCondInDesign, TempEvapOutDesign ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TempDesCondIn = TempCondInDesign;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).TempDesEvapOut = TempEvapOutDesign;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

				// Exhaust Fired Absorption Chiller

			} else if ( SELECT_CASE_var1 == TypeOf_Chiller_ExhFiredAbsorption ) {
				SimExhaustAbsorber( EquipType, EquipName, EquipFlowCtrl, EquipNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).NodeNumIn, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Chiller Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for Chiller=" + trim( EquipType ) );
				ShowContinueError( "..Chiller Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

			//HEAT PUMPS
		} else if ( SELECT_CASE_var == GenEquipTypes_HeatPump ) {
			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_HPWaterPECooling ) {
				SimHPWatertoWaterCOOLING( EquipType, EquipName, EquipNum, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, LoopNum ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_HPWaterPEHeating ) {
				SimHPWatertoWaterHEATING( EquipType, EquipName, EquipNum, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, LoopNum ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_HPWaterEFCooling ) {
				SimHPWatertoWaterSimple( EquipType, EquipTypeNum, EquipName, EquipNum, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, LoopNum ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_HPWaterEFHeating ) {
				SimHPWatertoWaterSimple( EquipType, EquipTypeNum, EquipName, EquipNum, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, LoopNum ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_HeatPumpVRF ) {

				SimVRFCondenserPlant( EquipType, EquipTypeNum, EquipName, EquipNum, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, LoopNum ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Heat Pump Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for HeatPump=" + trim( EquipType ) );
				ShowContinueError( "..HeatPump Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

			//TOWERS
		} else if ( SELECT_CASE_var == GenEquipTypes_CoolingTower ) {

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			//TOWERS
			if ( SELECT_CASE_var1 == TypeOf_CoolingTower_SingleSpd ) {

				SimTowers( EquipType, EquipName, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_CoolingTower_TwoSpd ) {

				SimTowers( EquipType, EquipName, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_CoolingTower_VarSpd ) { // 'CoolingTower:VariableSpeed'

				SimTowers( EquipType, EquipName, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_CoolingTower_VarSpdMerkel ) {

				SimTowers( EquipType, EquipName, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac );
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}
			} else {
				ShowSevereError( "SimPlantEquip: Invalid Tower Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for Cooling Tower=" + trim( EquipType ) );
				ShowContinueError( "..Tower Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

			//FLUID COOLERS
		} else if ( SELECT_CASE_var == GenEquipTypes_FluidCooler ) {

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			//FluidCoolerS
			if ( SELECT_CASE_var1 == TypeOf_FluidCooler_SingleSpd ) {

				SimFluidCoolers( EquipType, EquipName, EquipNum, RunFlag, InitLoopEquip, MaxLoad, MinLoad, OptLoad ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_FluidCooler_TwoSpd ) {

				SimFluidCoolers( EquipType, EquipName, EquipNum, RunFlag, InitLoopEquip, MaxLoad, MinLoad, OptLoad ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
			} else {
				ShowSevereError( "SimPlantEquip: Invalid FluidCooler Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for Fluid Cooler=" + trim( EquipType ) );
				ShowContinueError( "..Fluid Cooler Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

		} else if ( SELECT_CASE_var == GenEquipTypes_EvapFluidCooler ) {

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			//EvapFluidCoolers
			if ( SELECT_CASE_var1 == TypeOf_EvapFluidCooler_SingleSpd ) {

				SimEvapFluidCoolers( EquipType, EquipName, EquipNum, RunFlag, InitLoopEquip, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_EvapFluidCooler_TwoSpd ) {

				SimEvapFluidCoolers( EquipType, EquipName, EquipNum, RunFlag, InitLoopEquip, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
			} else {
				ShowSevereError( "SimPlantEquip: Invalid EvapFluidCooler Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for Fluid Cooler=" + trim( EquipType ) );
				ShowContinueError( "..Fluid Cooler Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

			//BOILERS
		} else if ( SELECT_CASE_var == GenEquipTypes_Boiler ) {
			{ auto const SELECT_CASE_var1( EquipTypeNum );
			if ( SELECT_CASE_var1 == TypeOf_Boiler_Simple ) {
				SimBoiler( EquipType, EquipName, EquipFlowCtrl, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_Boiler_Steam ) {
				SimSteamBoiler( EquipType, EquipName, EquipFlowCtrl, EquipNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Boiler Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for Boiler=" + trim( EquipType ) );
				ShowContinueError( "..Boiler Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

			//WATER HEATER
		} else if ( SELECT_CASE_var == GenEquipTypes_WaterThermalTank ) {

			{ auto const SELECT_CASE_var1( EquipTypeNum );
			if ( ( SELECT_CASE_var1 == TypeOf_WtrHeaterMixed ) || ( SELECT_CASE_var1 == TypeOf_WtrHeaterStratified ) ) {
				SimWaterThermalTank( EquipTypeNum, EquipName, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration, LoopNum, LoopSideNum ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

				// HEAT PUMP WATER HEATER
			} else if ( SELECT_CASE_var1 == TypeOf_HeatPumpWtrHeater ) {
				SimWaterThermalTank( EquipTypeNum, EquipName, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration, LoopNum, LoopSideNum ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Water Heater Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for Water Heater=" + trim( EquipType ) );
				ShowContinueError( "..Water Thermal Tank Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

			//PURCHASED
		} else if ( SELECT_CASE_var == GenEquipTypes_Purchased ) {
			{ auto const SELECT_CASE_var1( EquipTypeNum );
			if ( SELECT_CASE_var1 == TypeOf_PurchChilledWater ) {
				SimOutsideEnergy( EquipType, EquipName, EquipFlowCtrl, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_PurchHotWater ) {
				SimOutsideEnergy( EquipType, EquipName, EquipFlowCtrl, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid District Energy Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for District Energy=" + trim( EquipType ) );
				ShowContinueError( "..District Cooling/Heating Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

		} else if ( SELECT_CASE_var == GenEquipTypes_HeatExchanger ) {

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_FluidToFluidPlantHtExchg ) {
				SimFluidHeatExchanger( LoopNum, LoopSideNum, EquipType, EquipName, EquipNum, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad );
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Heat Exchanger Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );

			}}

		} else if ( SELECT_CASE_var == GenEquipTypes_GroundHeatExchanger ) {
			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_GrndHtExchgVertical ) { // 'GROUND HEAT EXCHANGER:VERTICAL'
				SimGroundHeatExchangers( EquipType, EquipName, EquipNum, RunFlag, FirstHVACIteration, InitLoopEquip ); //DSU

				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_GrndHtExchgSurface ) { // 'GROUND HEAT EXCHANGER:SURFACE'
				SimSurfaceGroundHeatExchanger( EquipName, EquipNum, FirstHVACIteration, RunFlag, InitLoopEquip ); //DSU

				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_GrndHtExchgPond ) { // 'GROUND HEAT EXCHANGER:POND'
				SimPondGroundHeatExchanger( EquipName, EquipNum, FirstHVACIteration, RunFlag, InitLoopEquip, MaxLoad, MinLoad, OptLoad ); //DSU

				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;

				}

			} else if ( SELECT_CASE_var1 == TypeOf_GrndHtExchgHorizTrench ) {
				SimPipingSystemCircuit( EquipName, PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum, InitLoopEquip, FirstHVACIteration );

				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			}}
			// THERMAL STORAGE
		} else if ( SELECT_CASE_var == GenEquipTypes_ThermalStorage ) {

			// If component setpoint based control is active for this equipment
			// then reset CurLoad to original EquipDemand.
			// Allow negative CurLoad.  For cold storage this means the storage should
			// charge, for hot storage, this means the storage should discharge.
			if ( PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CurOpSchemeType == CompSetPtBasedSchemeType ) {
				CurLoad = PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).EquipDemand;
				if ( CurLoad != 0 ) RunFlag = true;
			}

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_TS_IceSimple ) {

				SimIceStorage( EquipType, EquipName, EquipNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad ); //DSU | ,EquipFlowCtrl

				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = 0.0;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = 0.0;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = 0.0;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_TS_IceDetailed ) {

				SimIceStorage( EquipType, EquipName, EquipNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad ); //DSU | ,EquipFlowCtrl

				// Not sure what this really needs to do here...
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = 0.0;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = 0.0;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = 0.0;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( ( SELECT_CASE_var1 == TypeOf_ChilledWaterTankMixed ) || ( SELECT_CASE_var1 == TypeOf_ChilledWaterTankStratified ) ) {
				SimWaterThermalTank( EquipTypeNum, EquipName, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration, LoopNum, LoopSideNum ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Chilled/Ice Thermal Storage Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for Thermal Storage=" + trim( EquipType ) );
				ShowContinueError( "..Chilled/Ice Thermal Storage Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

		} else if ( SELECT_CASE_var == GenEquipTypes_Valve ) {
			{ auto const SELECT_CASE_var1( EquipTypeNum );
			if ( SELECT_CASE_var1 == TypeOf_ValveTempering ) {
				SimPlantValves( EquipTypeNum, EquipName, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
			} else {
				ShowSevereError( "SimPlantEquip: Invalid Valve Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for Valves=" + trim( EquipType ) );
				ShowContinueError( "..Valve Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

		} else if ( SELECT_CASE_var == GenEquipTypes_Generator ) {
			// for heat recovery plant interactions.
			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_Generator_FCExhaust ) {
				SimFuelCellPlantHeatRecovery( EquipType, EquipName, TypeOf_Generator_FCExhaust, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_Generator_FCStackCooler ) {
				SimFuelCellPlantHeatRecovery( EquipType, EquipName, TypeOf_Generator_FCStackCooler, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_Generator_MicroCHP ) {
				SimMicroCHPPlantHeatRecovery( EquipType, EquipName, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_Generator_MicroTurbine ) {
				SimMTPlantHeatRecovery( EquipType, EquipName, TypeOf_Generator_MicroTurbine, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_Generator_ICEngine ) {
				SimICEPlantHeatRecovery( EquipType, EquipName, TypeOf_Generator_ICEngine, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_Generator_CTurbine ) {
				SimCTPlantHeatRecovery( EquipType, EquipName, TypeOf_Generator_CTurbine, EquipNum, RunFlag, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, FirstHVACIteration ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Generator Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );

			}}

			if ( InitLoopEquip && EquipNum == 0 ) {
				ShowSevereError( "InitLoop did not set Equipment Index for Generator=" + trim( EquipType ) );
				ShowContinueError( "..Generator Name=" + trim( EquipName ) + ", in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Previous condition causes termination." );
			}

		} else if ( SELECT_CASE_var == GenEquipTypes_LoadProfile ) { // DSU2 draft out InitLoopEquip on a demand side component
			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_PlantLoadProfile ) {
				SimulatePlantProfile( EquipType, EquipName, TypeOf_PlantLoadProfile, EquipNum, FirstHVACIteration, InitLoopEquip );
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;

				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Load Profile Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

		} else if ( SELECT_CASE_var == GenEquipTypes_DemandCoil ) { //DSU3
			// for now these are place holders, the sim routines are called from other places, unclear if we need
			//  to call an update routine, or if air-side updates are sufficient.  this is where plant updates would be called from

			{ auto const SELECT_CASE_var1( EquipTypeNum ); //DSU3

			if ( SELECT_CASE_var1 == TypeOf_CoilWaterCooling ) {
				//          CALL UpdateWaterToAirCoilPlantConnection(EquipTypeNum,EquipName,EquipFlowCtrl,LoopNum,LoopSideNum,EquipNum,&
				//                                                   FirstHVACIteration, InitLoopEquip)
				//          IF(InitLoopEquip)THEN
				//            PlantLoop(LoopNum)%LoopSide(LoopSideNum)%Branch(BranchNum)%Comp(Num)%CompNum =  EquipNum
				//          ENDIF
			} else if ( SELECT_CASE_var1 == TypeOf_CoilWaterDetailedFlatCooling ) {
				//          CALL UpdateWaterToAirCoilPlantConnection(EquipTypeNum,EquipName,EquipFlowCtrl,LoopNum,LoopSideNum,EquipNum,&
				//                                                   FirstHVACIteration, InitLoopEquip)
				//          IF(InitLoopEquip)THEN
				//            PlantLoop(LoopNum)%LoopSide(LoopSideNum)%Branch(BranchNum)%Comp(Num)%CompNum =  EquipNum
				//          ENDIF
			} else if ( SELECT_CASE_var1 == TypeOf_CoilWaterSimpleHeating ) {
				//!          CALL UpdateWaterToAirCoilPlantConnection(EquipTypeNum,EquipName,EquipFlowCtrl,LoopNum,LoopSideNum,EquipNum,&
				//!                                                   FirstHVACIteration, InitLoopEquip)
				//!
				//          IF(InitLoopEquip)THEN
				//            PlantLoop(LoopNum)%LoopSide(LoopSideNum)%Branch(BranchNum)%Comp(Num)%CompNum =  EquipNum
				//          ENDIF
			} else if ( SELECT_CASE_var1 == TypeOf_CoilSteamAirHeating ) {
				//CALL UpdateSteamToAirCoilPlantConnection(  )

			} else if ( SELECT_CASE_var1 == TypeOf_CoilWAHPHeatingEquationFit ) {

			} else if ( SELECT_CASE_var1 == TypeOf_CoilWAHPCoolingEquationFit ) {

			} else if ( SELECT_CASE_var1 == TypeOf_CoilVSWAHPHeatingEquationFit ) {

			} else if ( SELECT_CASE_var1 == TypeOf_CoilVSWAHPCoolingEquationFit ) {

			} else if ( SELECT_CASE_var1 == TypeOf_CoilWAHPHeatingParamEst ) {

			} else if ( SELECT_CASE_var1 == TypeOf_CoilWAHPCoolingParamEst ) {

			} else if ( SELECT_CASE_var1 == TypeOf_PackagedTESCoolingCoil ) {

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Load Coil Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );

			}} //DSU3

		} else if ( SELECT_CASE_var == GenEquipTypes_WaterUse ) {

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_WaterUseConnection ) {

				SimulateWaterUseConnection( EquipTypeNum, EquipName, EquipNum, InitLoopEquip, FirstHVACIteration );

				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}
			} else {
				ShowSevereError( "SimPlantEquip: Invalid Load Coil Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

		} else if ( SELECT_CASE_var == GenEquipTypes_SolarCollector ) {

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( ( SELECT_CASE_var1 == TypeOf_SolarCollectorFlatPlate ) || ( SELECT_CASE_var1 == TypeOf_SolarCollectorICS ) ) {

				SimSolarCollector( EquipTypeNum, EquipName, EquipNum, InitLoopEquip, FirstHVACIteration );

				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_PVTSolarCollectorFlatPlate ) {

				SimPVTcollectors( EquipNum, FirstHVACIteration, CalledFromPlantLoopEquipMgr, EquipName, InitLoopEquip );

				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Solar Collector Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

		} else if ( SELECT_CASE_var == GenEquipTypes_ZoneHVACDemand ) { //ZoneHVAC and air terminal models with direct plant connections
			// for now these are place holders, the sim routines are called from other places, unclear if we need
			//  to call an update routine, or if air-side updates are sufficient.  this is where plant updates would be called from

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_Baseboard_Conv_Water ) {

			} else if ( SELECT_CASE_var1 == TypeOf_Baseboard_Rad_Conv_Steam ) {

			} else if ( SELECT_CASE_var1 == TypeOf_Baseboard_Rad_Conv_Water ) {

			} else if ( SELECT_CASE_var1 == TypeOf_LowTempRadiant_VarFlow ) {

			} else if ( SELECT_CASE_var1 == TypeOf_LowTempRadiant_ConstFlow ) {

			} else if ( SELECT_CASE_var1 == TypeOf_CooledBeamAirTerminal ) {

			} else if ( SELECT_CASE_var1 == TypeOf_MultiSpeedHeatPumpRecovery ) {

			} else if ( SELECT_CASE_var1 == TypeOf_UnitarySystemRecovery ) {

			} else {

				ShowSevereError( "SimPlantEquip: Invalid ZoneHVAC Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );

			}}

		} else if ( SELECT_CASE_var == GenEquipTypes_Refrigeration ) {

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_RefrigSystemWaterCondenser ) {
				SimRefrigCondenser( EquipTypeNum, EquipName, EquipNum, FirstHVACIteration, InitLoopEquip );

				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_RefrigerationWaterCoolRack ) {
				SimRefrigCondenser( EquipTypeNum, EquipName, EquipNum, FirstHVACIteration, InitLoopEquip );

				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Refrigeration Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

		} else if ( SELECT_CASE_var == GenEquipTypes_PlantComponent ) {

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_PlantComponentUserDefined ) {

				SimUserDefinedPlantComponent( LoopNum, LoopSideNum, EquipType, EquipName, EquipNum, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad );
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else if ( SELECT_CASE_var1 == TypeOf_WaterSource ) {

				SimWaterSource( EquipName, EquipFlowCtrl, EquipNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac ); //DSU
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

			} else {
				//        CALL ShowSevereError('SimPlantEquip: Invalid Component Equipment Type='//TRIM(EquipType))
				//        CALL ShowContinueError('Occurs in Plant Loop='//TRIM(PlantLoop(LoopNum)%Name))
				//        CALL ShowFatalError('Preceding condition causes termination.')

			}}

		} else if ( SELECT_CASE_var == GenEquipTypes_CentralHeatPumpSystem ) {

			{ auto const SELECT_CASE_var1( EquipTypeNum );

			if ( SELECT_CASE_var1 == TypeOf_CentralGroundSourceHeatPump ) {

				SimCentralGroundSourceHeatPump( EquipName, EquipFlowCtrl, EquipNum, LoopNum, RunFlag, FirstHVACIteration, InitLoopEquip, CurLoad, MaxLoad, MinLoad, OptLoad, GetCompSizFac, SizingFac );
				if ( InitLoopEquip ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MaxLoad = MaxLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).MinLoad = MinLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).OptLoad = OptLoad;
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).CompNum = EquipNum;
				}

				if ( GetCompSizFac ) {
					PlantLoop( LoopNum ).LoopSide( LoopSideNum ).Branch( BranchNum ).Comp( Num ).SizFac = SizingFac;
				}

			} else {
				ShowSevereError( "SimPlantEquip: Invalid Central Heat Pump System Type=" + trim( EquipType ) );
				ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
				ShowFatalError( "Preceding condition causes termination." );
			}}

		} else {
			ShowSevereError( "SimPlantEquip: Invalid Equipment Type=" + trim( EquipType ) );
			ShowContinueError( "Occurs in Plant Loop=" + trim( PlantLoop( LoopNum ).Name ) );
			ShowFatalError( "Preceding condition causes termination." );
		}} // TypeOfEquip

	}

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

} // PlantLoopEquip

} // EnergyPlus