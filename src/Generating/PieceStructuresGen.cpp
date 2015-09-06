
// PieceStructuresGen.cpp

// Declares the cPieceStructuresGen class representing the PieceStructures finisher generator

#include "Globals.h"
#include "PieceStructuresGen.h"
#include "PrefabStructure.h"
#include "IniFile.h"






cPieceStructuresGen::cPieceStructuresGen(int a_Seed):
	m_Seed(a_Seed)
{
}





void cPieceStructuresGen::Initialize(cIniFile & a_IniFile, cBiomeGenPtr a_BiomeGen, cTerrainHeightGenPtr a_HeightGen)
{
	// Load each piecepool:
	auto Structures = StringSplitAndTrim(a_IniFile.GetValueSet("Generator", "PieceStructures", ""), ",");
	for (const auto & s: Structures)
	{
		auto gen = std::make_shared<cGen>(m_Seed, a_BiomeGen, a_HeightGen);
		if (gen->LoadFromFile(Printf("Prefabs%cStructures%c%s.cubeset", cFile::PathSeparator, cFile::PathSeparator, s.c_str())))
		{
			m_Gens.push_back(gen);
		}
	}

	// Report a warning if no generators available:
	if (m_Gens.empty())
	{
		LOGWARNING("The PieceStructures generator is enabled, but is not set for any valid structures.");
	}
}





void cPieceStructuresGen::GenFinish(cChunkDesc & a_Chunk)
{
	for (auto & gen: m_Gens)
	{
		gen->GenFinish(a_Chunk);
	}
}





////////////////////////////////////////////////////////////////////////////////
// cPieceStructuresGen::cGen:

cPieceStructuresGen::cGen::cGen(int a_Seed, cBiomeGenPtr a_BiomeGen, cTerrainHeightGenPtr a_HeightGen):
	Super(a_Seed),
	m_BiomeGen(a_BiomeGen),
	m_HeightGen(a_HeightGen),
	m_MaxDepth(5)
{
}





bool cPieceStructuresGen::cGen::LoadFromFile(const AString & a_FileName)
{
	// Load the piecepool from the file, log any warnings:
	if (!m_Pool.LoadFromFile(a_FileName, true))
	{
		return false;
	}
	if (NoCaseCompare(m_Pool.GetIntendedUse(), "PieceStructures") != 0)
	{
		LOGWARNING("PieceStructures generator: File %s is intended for use in \"%s\", rather than piece structures. Loading the file, but the generator may behave unexpectedly.",
			a_FileName.c_str(), m_Pool.GetIntendedUse().c_str()
		);
	}
	m_Pool.AssignGens(m_Seed, m_BiomeGen, m_HeightGen);

	// Apply generator params from the piecepool into the generator:
	auto & generatorParams = m_Pool.GetGeneratorParams();
	SetGeneratorParams(generatorParams);
	m_MaxDepth = GetStringMapInteger<int>(generatorParams, "MaxDepth", m_MaxDepth);

	return true;
}





cGridStructGen::cStructurePtr cPieceStructuresGen::cGen::CreateStructure(int a_GridX, int a_GridZ, int a_OriginX, int a_OriginZ)
{
	cBFSPieceGenerator pg(m_Pool, m_Seed);
	cPlacedPieces outPieces;
	pg.PlacePieces(a_OriginX, a_OriginZ, m_MaxDepth, outPieces);
	return std::make_shared<cPrefabStructure>(a_GridX, a_GridZ, a_OriginX, a_OriginZ, outPieces, m_HeightGen);
}




