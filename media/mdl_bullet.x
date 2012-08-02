xof 0303txt 0032

template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template VertexDuplicationIndices {
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}


Mesh Mesh_Unnamed {
 10;
 -0.025006;1.571226;-3.134845;,
 -0.025006;1.571226;-3.134845;,
 1.696565;-1.324192;-3.134845;,
 1.696565;-1.324192;-3.134845;,
 -1.746577;-1.324192;-3.134845;,
 -1.746577;-1.324192;-3.134845;,
 -0.025006;0.002789;10.924500;,
 -0.025006;-0.020991;-8.262076;,
 -0.025006;-0.020991;-8.262076;,
 -0.025006;-0.020991;-8.262076;;
 6;
 3;4,6,0;,
 3;5,2,6;,
 3;7,1,3;,
 3;8,2,5;,
 3;9,4,0;,
 3;3,1,6;;

 MeshNormals {
  10;
  -0.858456;0.510425;-0.050200;,
  0.858456;0.510425;-0.050200;,
  0.000000;-0.997006;-0.077321;,
  0.858456;0.510425;-0.050200;,
  -0.858456;0.510425;-0.050200;,
  0.000000;-0.997006;-0.077321;,
  0.000000;0.118996;0.992895;,
  0.848915;0.504752;-0.156746;,
  0.000000;-0.969184;-0.246340;,
  -0.848915;0.504752;-0.156746;;
  6;
  3;4,6,0;,
  3;5,2,6;,
  3;7,1,3;,
  3;8,2,5;,
  3;9,4,0;,
  3;3,1,6;;
 }

 VertexDuplicationIndices {
  10;
  8;
  0,
  0,
  2,
  2,
  4,
  4,
  6,
  7,
  7,
  7;
 }

 MeshMaterialList {
  1;
  6;
  0,
  0,
  0,
  0,
  0,
  0;

  Material {
   0.784314;0.784314;0.784314;1.000000;;
   51.200001;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
}
