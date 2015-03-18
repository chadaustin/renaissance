module Main where

import Data.Monoid
import Data.Foldable
import qualified Data.Set as Set

data ScalarType = SBool | SInt | SFloat
                deriving (Show, Eq, Ord)
data Arity = A2 | A3 | A4
           deriving (Show, Eq, Ord)
data Type = TScalar ScalarType | TVec Arity ScalarType | Matrix Arity | Sampler2D | SamplerCube
          deriving (Show, Eq, Ord)
data Frequency = Constant | Batch | Vertex | Fragment
data FT = FT Frequency Type

data Uniform = Uniform Type String
             deriving (Show, Eq, Ord)
data Attribute = Attribute Type String
               deriving (Show, Eq, Ord)

{-
data Output t f = Fragment 
data VertexShader = VertexShader
                    { gl_Position :: Output Vec4f
                    }
-}

float = TScalar SFloat
bool = TScalar SBool
int = TScalar SInt
vec2 = TVec A2 SFloat
vec3 = TVec A3 SFloat
vec4 = TVec A4 SFloat
bvec2 = TVec A2 SBool
bvec3 = TVec A3 SBool
bvec4 = TVec A4 SBool
ivec2 = TVec A2 SInt
ivec3 = TVec A3 SInt
ivec4 = TVec A4 SInt
mat2 = Matrix A2
mat3 = Matrix A3
mat4 = Matrix A4

data Expression = Mult Expression Expression
                | ReadUniform Uniform
                | ReadAttribute Attribute
                deriving (Show, Eq, Ord)

class AsExpr a where
    asExpr :: a -> Expression

instance AsExpr Expression where
    asExpr = id

instance AsExpr Uniform where
    asExpr = ReadUniform

instance AsExpr Attribute where
    asExpr = ReadAttribute

data VSOutput = VSOutput String Type Expression

mult :: (AsExpr a, AsExpr b) => a -> b -> Expression
mult lhs rhs = Mult (asExpr lhs) (asExpr rhs)
infixl 7 `mult`

findUniforms :: VSOutput -> Set.Set Uniform
findUniforms (VSOutput _ _ expr) = find' expr
  where
    find' (Mult lhs rhs) = Set.union (find' lhs) (find' rhs)
    find' (ReadUniform u) = Set.singleton u
    find' (ReadAttribute _) = Set.empty

findAttributes :: VSOutput -> Set.Set Attribute
findAttributes (VSOutput _ _ expr) = find' expr
  where
    find' (Mult lhs rhs) = Set.union (find' lhs) (find' rhs)
    find' (ReadUniform _) = Set.empty
    find' (ReadAttribute a) = Set.singleton a

findAllUniforms :: [VSOutput] -> Set.Set Uniform
findAllUniforms outputs = Set.unions $ fmap findUniforms outputs

findAllAttributes :: [VSOutput] -> Set.Set Attribute
findAllAttributes outputs = Set.unions $ fmap findAttributes outputs

genCode :: Expression -> String
genCode (ReadUniform (Uniform _ n)) = n
genCode (ReadAttribute (Attribute _ n)) = n
genCode (Mult lhs rhs) = "(" <> genCode lhs <> "*" <> genCode rhs <> ")"

genArity :: Arity -> String
genArity A2 = "2"
genArity A3 = "3"
genArity A4 = "4"

genType :: Type -> String
genType (TScalar SBool) = "bool"
genType (TScalar SInt) = "int"
genType (TScalar SFloat) = "float"
genType (TVec arity st) = vecPrefix st <> "vec" <> genArity arity
  where
    vecPrefix SBool = "b"
    vecPrefix SInt = "i"
    vecPrefix SFloat = ""
genType (Matrix arity) = "mat" <> genArity arity
genType Sampler2D = "sampler2D"
genType SamplerCube = "samplerCube"
         
genUniform :: Uniform -> String
genUniform (Uniform t name) = "uniform " <> genType t <> " " <> name <> ";\n"

genAttribute :: Attribute -> String
genAttribute (Attribute t name) = "attribute " <> genType t <> " " <> name <> ";\n"

genOutput :: VSOutput -> String
genOutput (VSOutput name _t e) = " " <> name <> "=" <> genCode e <> ";\n"

genShader :: [VSOutput] -> String
genShader outputs =
    (foldMap genUniform $ Set.toList $ findAllUniforms outputs) <>
    (foldMap genAttribute $ Set.toList $ findAllAttributes outputs) <>
    "void main() {\n" <>
    foldMap genOutput outputs <>
    "}\n"
    

main  :: IO ()
main = do
    let modelMatrix = Uniform mat4 "modelMatrix"
        viewMatrix = Uniform mat4 "viewMatrix"
        projMatrix = Uniform mat4 "projMatrix"
        position = Attribute vec4 "position"

    let gl_Position = projMatrix `mult` viewMatrix `mult` modelMatrix `mult` position
    
    putStrLn $ genShader [VSOutput "gl_Position" vec4 gl_Position]
