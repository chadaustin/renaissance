{-# LANGUAGE RecordWildCards, TypeFamilies, MultiParamTypeClasses, ExistentialQuantification, TypeSynonymInstances, FlexibleInstances, ScopedTypeVariables, GADTs #-}

module Main where

import Data.List
import Data.Monoid
import Data.Foldable
import qualified Data.Set as Set

data ScalarType = SBool | SInt | SFloat
                deriving (Show, Eq, Ord)
data Arity = A2 | A3 | A4
           deriving (Show, Eq, Ord)
data Type = Scalar ScalarType | Vec Arity ScalarType | Matrix Arity | Sampler2D | SamplerCube
          deriving (Show, Eq, Ord)
data Frequency = FConstant | FBatch | FVertex | FFragment
data FT = FT Frequency Type

data Uniform t = Uniform String
               | UniformArray Int String
               deriving (Show, Eq, Ord)
data Attribute t = Attribute String
                 | AttributeArray Int String
                 deriving (Show, Eq, Ord)
data Constant t = Constant t
                deriving (Show, Eq, Ord)

float, bool, int, vec2, vec3, vec4, bvec2, bvec3, bvec4, ivec2, ivec3, ivec4, mat2, mat3, mat4 :: Type
float = Scalar SFloat
bool = Scalar SBool
int = Scalar SInt
vec2 = Vec A2 SFloat
vec3 = Vec A3 SFloat
vec4 = Vec A4 SFloat
bvec2 = Vec A2 SBool
bvec3 = Vec A3 SBool
bvec4 = Vec A4 SBool
ivec2 = Vec A2 SInt
ivec3 = Vec A3 SInt
ivec4 = Vec A4 SInt
mat2 = Matrix A2
mat3 = Matrix A3
mat4 = Matrix A4

type Vec2 = (Float, Float)
type Vec3 = (Float, Float, Float)
type Vec4 = (Float, Float, Float, Float)
data Mat2
data Mat3
data Mat4 = Mat4
data Sampler2D
data SamplerCube

-- Proxy

data Proxy a = Proxy
    
-- getRuntimeType

class GetRuntimeType a where
    getRuntimeType :: Proxy a -> Type
instance GetRuntimeType Bool where
    getRuntimeType _ = bool
instance GetRuntimeType Int where
    getRuntimeType _ = int
instance GetRuntimeType Float where
    getRuntimeType _ = float
instance GetRuntimeType Vec2 where
    getRuntimeType _ = vec2
instance GetRuntimeType Vec3 where
    getRuntimeType _ = vec3
instance GetRuntimeType Vec4 where
    getRuntimeType _ = vec4
instance GetRuntimeType Mat4 where
    getRuntimeType _ = mat4
instance GetRuntimeType Sampler2D where
    getRuntimeType _ = Sampler2D

class RenderConstant a where
    renderConstant :: a -> String
instance RenderConstant Bool where
    renderConstant t = if t then "true" else "false"
instance RenderConstant Float where
    renderConstant = show
instance RenderConstant Mat4 where
    renderConstant = const "Mat4"
instance RenderConstant Vec2 where
    renderConstant (x, y) = "vec2(" <> show x <> "," <> show y <> ")"
instance RenderConstant Vec3 where
    renderConstant (x, y, z) = "vec3(" <> show x <> "," <> show y <> "," <> show z <> ")"
instance RenderConstant Vec4 where
    renderConstant (x, y, z, w) = "vec4(" <> show x <> "," <> show y <> "," <> show z <> "," <> show w <> ")"

-- special fragment inputs: vec4 gl_FragCoord, bool gl_FrontFacing, vec2 gl_PointCoord

data Expression t where
    Call :: String -> [SomeExpression] -> Expression t
    Mult :: Expression a -> Expression b -> Expression (MultResult a b)
    IfThenElse :: Expression Bool -> Expression a -> Expression a -> Expression a
    Swizzle :: String -> Expression a -> Expression b
    ReadUniform :: GetRuntimeType t => Uniform t -> Expression t
    ReadAttribute :: GetRuntimeType t => Attribute t -> Expression t
    ReadConstant :: RenderConstant t => Constant t -> Expression t

data SomeExpression = forall a. SomeExpression (Expression a)

class AsExpr a where
    asExpr :: (RenderConstant t, GetRuntimeType t) => a t -> Expression t

instance AsExpr Expression where
    asExpr = id

instance AsExpr Uniform where
    asExpr = ReadUniform

instance AsExpr Attribute where
    asExpr = ReadAttribute

instance AsExpr Constant where
    asExpr = ReadConstant

data VSOutput t = VSOutput String (Expression t)
type FSOutput = VSOutput
--data FSOutput t = FSOutput String (Expression t)

class HasFourComponents a where
    toExpressionList :: a -> [SomeExpression]
instance HasFourComponents (Expression Float, Expression Float, Expression Float, Expression Float) where
    toExpressionList (a, b, c, d) = [SomeExpression a, SomeExpression b, SomeExpression c, SomeExpression d]
instance HasFourComponents (Expression Float, Expression Vec2, Expression Float) where
    toExpressionList (a, b, c) = [SomeExpression a, SomeExpression b, SomeExpression c]
instance HasFourComponents (Expression Vec3, Expression Float) where
    toExpressionList (a, b) = [SomeExpression a, SomeExpression b]

makeVec4 :: HasFourComponents a => a -> Expression Vec4
makeVec4 a = Call "vec4" $ toExpressionList a

-- texture2D
texture2D :: AsExpr a => Uniform Sampler2D -> a Vec2 -> Expression Vec4
texture2D sampler coord = Call "texture2D" [SomeExpression $ ReadUniform sampler, SomeExpression $ asExpr coord]

-- mult

-- multiplication
-- float * vec a
-- vec a * float
-- float * mat a
-- mat a * float
-- int * ivec a
-- ivec a * int
-- mat a * vec a
-- vec * vec
-- mat * mat

type family MultResult a b
type instance MultResult Mat4 Vec4 = Vec4
type instance MultResult Mat4 Mat4 = Mat4

mult :: (AsExpr e, AsExpr f, GetRuntimeType a, RenderConstant a, GetRuntimeType b, RenderConstant b) => e a -> f b -> Expression (MultResult a b)
mult lhs rhs = Mult (asExpr lhs) (asExpr rhs)
infixl 7 `mult`

ifThenElse :: (AsExpr p, AsExpr t, AsExpr f, RenderConstant a, GetRuntimeType a) => p Bool -> t a -> f a -> Expression a
ifThenElse p t f = IfThenElse (asExpr p) (asExpr t) (asExpr f)

data UniformDesc = UniformDesc Type String
                 | UniformArrayDesc Type Int String
                 deriving (Eq, Ord)
data AttributeDesc = AttributeDesc Type String
                   | AttributeArrayDesc Type Int String
                   deriving (Eq, Ord)

toUniformDesc :: forall t. GetRuntimeType t => Uniform t -> UniformDesc
toUniformDesc (Uniform name) = UniformDesc (getRuntimeType (Proxy :: Proxy t)) name

toAttributeDesc :: forall t. GetRuntimeType t => Attribute t -> AttributeDesc
toAttributeDesc (Attribute name) = AttributeDesc (getRuntimeType (Proxy :: Proxy t)) name

findUniforms :: VSOutput t -> Set.Set UniformDesc
findUniforms (VSOutput _ expr) = find' expr
  where
    find' :: Expression a -> Set.Set UniformDesc
    find' (Mult lhs rhs) = Set.union (find' lhs) (find' rhs)
    find' (Call _ args) = Set.unions $ fmap (\(SomeExpression e) -> find' e) args
    find' (IfThenElse pred' ifTrue ifFalse) = Set.unions [find' pred', find' ifTrue, find' ifFalse]
    find' (Swizzle _ e) = find' e
    find' (ReadUniform u) = Set.singleton $ toUniformDesc u
    find' (ReadAttribute _) = Set.empty
    find' (ReadConstant _) = Set.empty

findAttributes :: VSOutput t -> Set.Set AttributeDesc
findAttributes (VSOutput _ expr) = find' expr
  where
    find' :: Expression a -> Set.Set AttributeDesc
    find' (Mult lhs rhs) = Set.union (find' lhs) (find' rhs)
    find' (Call _ args) = Set.unions $ fmap (\(SomeExpression e) -> find' e) args
    find' (IfThenElse pred' ifTrue ifFalse) = Set.unions [find' pred', find' ifTrue, find' ifFalse]
    find' (Swizzle _ e) = find' e
    find' (ReadUniform _) = Set.empty
    find' (ReadAttribute a) = Set.singleton $ toAttributeDesc a
    find' (ReadConstant _) = Set.empty
    
findAllUniforms :: [VSOutput t] -> Set.Set UniformDesc
findAllUniforms outputs = Set.unions $ fmap findUniforms outputs

findAllAttributes :: [VSOutput t] -> Set.Set AttributeDesc
findAllAttributes outputs = Set.unions $ fmap findAttributes outputs

genCode :: Expression a -> String
genCode (Call name args) = "(" <> name <> "(" <> intercalate "," (fmap (\(SomeExpression e) -> genCode e) args) <> "))"
genCode (Mult lhs rhs) = "(" <> genCode lhs <> "*" <> genCode rhs <> ")"
genCode (IfThenElse pred' lhs rhs) = "(" <> genCode pred' <> "?" <> genCode lhs <> ":" <> genCode rhs <> ")"
genCode (Swizzle name e) = "(" <> genCode e <> "." <> name <> ")"
genCode (ReadUniform (Uniform n)) = n
genCode (ReadAttribute (Attribute n)) = n
genCode (ReadConstant (Constant k)) = renderConstant k

genArity :: Arity -> String
genArity A2 = "2"
genArity A3 = "3"
genArity A4 = "4"

genType :: Type -> String
genType (Scalar SBool) = "bool"
genType (Scalar SInt) = "int"
genType (Scalar SFloat) = "float"
genType (Vec arity st) = vecPrefix st <> "vec" <> genArity arity
  where
    vecPrefix SBool = "b"
    vecPrefix SInt = "i"
    vecPrefix SFloat = ""
genType (Matrix arity) = "mat" <> genArity arity
genType Sampler2D = "sampler2D"
genType SamplerCube = "samplerCube"
         
genUniform :: UniformDesc -> String
genUniform (UniformDesc t name) = "uniform " <> genType t <> " " <> name <> ";\n"
genUniform (UniformArrayDesc t size name) = "uniform " <> genType t <> " " <> name <> "[" <> show size <> "];\n"

genAttribute :: AttributeDesc -> String
genAttribute (AttributeDesc t name) = "attribute " <> genType t <> " " <> name <> ";\n"
genAttribute (AttributeArrayDesc t size name) = "attribute " <> genType t <> " " <> name <> "[" <> show size <> ";\n"

genOutput :: VSOutput t -> String
genOutput (VSOutput name e) = " " <> name <> "=" <> genCode e <> ";\n"

genShader :: [VSOutput t] -> String
genShader outputs =
    (foldMap genUniform $ Set.toList $ findAllUniforms outputs) <>
    (foldMap genAttribute $ Set.toList $ findAllAttributes outputs) <>
    "void main() {\n" <>
    foldMap genOutput outputs <>
    "}\n"

emitProgram :: Program -> IO ()
emitProgram Program{..} = do
    putStrLn "// vertex shader"
    putStrLn $ genShader [gl_Position]

    putStrLn "// fragment shader"
    putStrLn $ genShader [gl_FragColor]

data Program = Program
               { gl_Position :: VSOutput Vec4
               , gl_PointSize :: Maybe (VSOutput Float)

               , gl_FragColor :: FSOutput Vec4
               , gl_FragData :: [FSOutput Vec4]
               , gl_Discard :: Maybe (FSOutput Bool)
               }

makeBasicProgram :: AsExpr a => a Vec4 -> a Vec4 -> Program
makeBasicProgram gl_Position gl_FragColor =
    Program
    { gl_Position = VSOutput "gl_Position" (asExpr gl_Position)
    , gl_PointSize = Nothing
    , gl_FragColor = VSOutput "gl_FragColor" (asExpr gl_FragColor)
    , gl_FragData = []
    , gl_Discard = Nothing
    }

-- swizzles

type family VectorElement a
type instance VectorElement Vec2 = Float
type instance VectorElement Vec3 = Float
type instance VectorElement Vec4 = Float

class HasTwoComponents a
instance HasTwoComponents Vec2
instance HasTwoComponents Vec3
instance HasTwoComponents Vec4

class HasThreeComponents a
instance HasThreeComponents Vec3
instance HasThreeComponents Vec4

type family Vec3Of a
type instance Vec3Of Float = Vec3

x' :: (AsExpr a, HasTwoComponents b, GetRuntimeType b, RenderConstant b) => a b -> Expression (VectorElement b)
x' v = Swizzle "x" (asExpr v)
y' :: (AsExpr a, HasTwoComponents b, GetRuntimeType b, RenderConstant b) => a b -> Expression (VectorElement b)
y' v = Swizzle "y" (asExpr v)
w' :: (AsExpr a, HasTwoComponents b, GetRuntimeType b, RenderConstant b) => a b -> Expression (VectorElement b)
w' v = Swizzle "w" (asExpr v)

xyz' :: (AsExpr a, HasThreeComponents b, GetRuntimeType b, RenderConstant b) => a b -> Expression (Vec3Of (VectorElement b))
xyz' v = Swizzle "xyz" (asExpr v)

main  :: IO ()
main = do
    let modelMatrix = Uniform "modelMatrix" :: Uniform Mat4
        viewMatrix = Uniform "viewMatrix" :: Uniform Mat4
        projMatrix = Uniform "projMatrix" :: Uniform Mat4
        position = Attribute "position" :: Attribute Vec4
        texCoord = Attribute "texCoord" :: Attribute Vec2
        diffuseMap0 = Uniform "diffuseMap0" :: Uniform Sampler2D

    let diffuse = texture2D diffuseMap0 texCoord

    let gl_Position = projMatrix `mult` viewMatrix `mult` modelMatrix `mult` position
    let isWhite = Constant True
    let rgb = xyz' diffuse
    let whiteCase = makeVec4 (asExpr $ rgb, asExpr $ w' diffuse)
    let gl_FragColor = ifThenElse isWhite whiteCase (Constant (0, 0, 0, 0))
    let program = makeBasicProgram gl_Position gl_FragColor

    emitProgram program
    
