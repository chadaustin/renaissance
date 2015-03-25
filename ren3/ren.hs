{-# LANGUAGE RecordWildCards, TypeFamilies, MultiParamTypeClasses,
    ExistentialQuantification, TypeSynonymInstances, FlexibleInstances,
    ScopedTypeVariables, GADTs, ConstraintKinds, FunctionalDependencies,
    FlexibleContexts, Rank2Types #-}

module Main where

import GHC.Prim (Constraint)
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
    BinaryOp :: String -> Expression a -> Expression a -> Expression a
    UnaryOp :: String -> Expression a -> Expression a
    IfThenElse :: Expression Bool -> Expression a -> Expression a -> Expression a
    Swizzle :: String -> Expression a -> Expression b
    ReadUniform :: GetRuntimeType t => Uniform t -> Expression t
    ReadAttribute :: GetRuntimeType t => Attribute t -> Expression t
    ReadConstant :: RenderConstant t => Constant t -> Expression t

traverse :: forall r m. Monoid m => (forall a. Expression a -> m) -> Expression r -> m
traverse traversal root = traverse' root
  where
    traverse' :: Expression b -> m
    traverse' expr = traversal expr <> recurse expr

    recurse :: Expression b -> m
    recurse (Call _ args) = foldMap (\(SomeExpression e) -> traverse' e) args
    recurse (Mult lhs rhs) = traverse' lhs <> traverse' rhs
    recurse (BinaryOp _ lhs rhs) = traverse' lhs <> traverse' rhs
    recurse (UnaryOp _ e) = traverse' e
    recurse (IfThenElse pred' ifTrue ifFalse) = traverse' pred' <> traverse' ifTrue <> traverse' ifFalse
    recurse (Swizzle _ e) = traverse' e
    recurse (ReadUniform _) = mempty
    recurse (ReadAttribute _) = mempty
    recurse (ReadConstant _) = mempty

data SomeExpression = forall a. SomeExpression (Expression a)

class ToExpr a (b :: * -> Constraint) | a -> b where
    toExpr :: b t => a t -> Expression t

class NoConstraint t
instance NoConstraint t

instance ToExpr Expression NoConstraint where
    toExpr = id

instance ToExpr Uniform GetRuntimeType where
    toExpr = ReadUniform

instance ToExpr Attribute GetRuntimeType where
    toExpr = ReadAttribute

instance ToExpr Constant RenderConstant where
    toExpr = ReadConstant

data VSOutput t = VSOutput String (Expression t)
type FSOutput = VSOutput
--data FSOutput t = FSOutput String (Expression t)

class HasFourComponents a
instance HasFourComponents (Float, Float, Float, Float)
instance HasFourComponents (Float, Vec2, Float)
instance HasFourComponents (Vec3, Float)

class MakeVec4 a where
    toExpressionList :: a -> [SomeExpression]
instance HasFourComponents (a, b) => MakeVec4 (Expression a, Expression b) where
    toExpressionList (a, b) = [SomeExpression a, SomeExpression b]
instance HasFourComponents (a, b, c) => MakeVec4 (Expression a, Expression b, Expression c) where
    toExpressionList (a, b, c) = [SomeExpression a, SomeExpression b, SomeExpression c]
instance HasFourComponents (a, b, c, d) => MakeVec4 (Expression a, Expression b, Expression c, Expression d) where
    toExpressionList (a, b, c, d) = [SomeExpression a, SomeExpression b, SomeExpression c, SomeExpression d]

makeVec4 :: MakeVec4 a => a -> Expression Vec4
makeVec4 a = Call "vec4" $ toExpressionList a

-- texture2D
texture2D :: (ToExpr a c, c Vec2) => Uniform Sampler2D -> a Vec2 -> Expression Vec4
texture2D sampler coord = Call "texture2D" [SomeExpression $ ReadUniform sampler, SomeExpression $ toExpr coord]

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
type instance MultResult Float Float = Float
type instance MultResult Mat4 Vec4 = Vec4
type instance MultResult Mat4 Mat4 = Mat4
type instance MultResult Vec3 Float = Vec3

mult :: (ToExpr e c1, ToExpr f c2, c1 a, c2 b) => e a -> f b -> Expression (MultResult a b)
mult lhs rhs = Mult (toExpr lhs) (toExpr rhs)
infixl 7 `mult`

ifThenElse :: (ToExpr p c1, ToExpr t c2, ToExpr f c3, c1 Bool, c2 a, c3 a) => p Bool -> t a -> f a -> Expression a
ifThenElse p t f = IfThenElse (toExpr p) (toExpr t) (toExpr f)

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
findUniforms (VSOutput _ expr) = traverse find' expr
  where
    find' :: Expression a -> Set.Set UniformDesc
    find' (ReadUniform u) = Set.singleton $ toUniformDesc u
    find' _ = Set.empty

findAttributes :: VSOutput t -> Set.Set AttributeDesc
findAttributes (VSOutput _ expr) = traverse find' expr
  where
    find' :: Expression a -> Set.Set AttributeDesc
    find' (ReadAttribute a) = Set.singleton $ toAttributeDesc a
    find' _ = Set.empty
    
findAllUniforms :: [VSOutput t] -> Set.Set UniformDesc
findAllUniforms outputs = Set.unions $ fmap findUniforms outputs

findAllAttributes :: [VSOutput t] -> Set.Set AttributeDesc
findAllAttributes outputs = Set.unions $ fmap findAttributes outputs

genCode :: Expression a -> String
genCode (Call name args) = "(" <> name <> "(" <> intercalate "," (fmap (\(SomeExpression e) -> genCode e) args) <> "))"
genCode (BinaryOp name lhs rhs) = "(" <> genCode lhs <> name <> genCode rhs <> ")"
genCode (UnaryOp name e) = "(" <> name <> genCode e <> ")"
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

makeBasicProgram :: (ToExpr a c, c Vec4) => a Vec4 -> a Vec4 -> Program
makeBasicProgram gl_Position gl_FragColor =
    Program
    { gl_Position = VSOutput "gl_Position" (toExpr gl_Position)
    , gl_PointSize = Nothing
    , gl_FragColor = VSOutput "gl_FragColor" (toExpr gl_FragColor)
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

x' :: (ToExpr a c, HasTwoComponents b, c b) => a b -> Expression (VectorElement b)
x' v = Swizzle "x" (toExpr v)
y' :: (ToExpr a c, HasTwoComponents b, c b) => a b -> Expression (VectorElement b)
y' v = Swizzle "y" (toExpr v)
z' :: (ToExpr a c, HasTwoComponents b, c b) => a b -> Expression (VectorElement b)
z' v = Swizzle "z" (toExpr v)
w' :: (ToExpr a c, HasTwoComponents b, c b) => a b -> Expression (VectorElement b)
w' v = Swizzle "w" (toExpr v)

xyz' :: (ToExpr a c, HasThreeComponents b, c b) => a b -> Expression (Vec3Of (VectorElement b))
xyz' v = Swizzle "xyz" (toExpr v)

neg :: (ToExpr a c, c b) => a b -> Expression b
neg v = UnaryOp "-" (toExpr v)

clamp :: (ToExpr a c1, c1 Float, ToExpr b c2, c2 Float, ToExpr c c3, c3 Float) => a Float -> b Float -> c Float -> Expression Float
clamp x minV maxV = Call "clamp" [SomeExpression $ toExpr x, SomeExpression $ toExpr minV, SomeExpression $ toExpr maxV]

mix :: (ToExpr a c1, c1 Vec3, ToExpr b c2, c2 Vec3, ToExpr c c3, c3 Float) => a Vec3 -> b Vec3 -> c Float -> Expression Vec3
mix x y a = Call "mix" [SomeExpression $ toExpr x, SomeExpression $ toExpr y, SomeExpression $ toExpr a]

minus :: (ToExpr a c1, c1 x, ToExpr b c2, c2 x) => a x -> b x -> Expression x
minus lhs rhs = BinaryOp "-" (toExpr lhs) (toExpr rhs)

dividedBy :: (ToExpr a c1, c1 Float, ToExpr b c2, c2 Float) => a Float -> b Float -> Expression Float
dividedBy lhs rhs = BinaryOp "/" (toExpr lhs) (toExpr rhs)

main  :: IO ()
main = do
    let modelMatrix = Uniform "modelMatrix" :: Uniform Mat4
        viewMatrix = Uniform "viewMatrix" :: Uniform Mat4
        projMatrix = Uniform "projMatrix" :: Uniform Mat4
        position = Attribute "position" :: Attribute Vec4
        texCoord = Attribute "texCoord" :: Attribute Vec2

        positionWorld = modelMatrix `mult` position
        positionView = viewMatrix `mult` positionWorld

        uFogMultiplier = Uniform "fogMultiplier" :: Uniform Float
        uFogColor = Uniform "fogColor" :: Uniform Vec4
        uFogParams = Uniform "fogParams" :: Uniform Vec4

        fogNear = x' uFogParams
        fogFar = y' uFogParams
        fogDensity = z' uFogParams
        
        diffuseMap0 = Uniform "diffuseMap0" :: Uniform Sampler2D

    let fogCoord = neg $ x' positionView
    let fogInvRange = Constant 1.0 `dividedBy` (fogFar `minus` fogNear)
    let fogIntensity = clamp (fogDensity `mult` uFogMultiplier `mult` (fogCoord `minus` fogNear) `mult` fogInvRange) (Constant 0.0) (Constant 1.0)

    let diffuse = texture2D diffuseMap0 texCoord

    let gl_Position = projMatrix `mult` viewMatrix `mult` modelMatrix `mult` position
    let isWhite = Constant True
    let alpha = w' diffuse
    let rgb = mix (xyz' diffuse) (xyz' uFogColor `mult` alpha) fogIntensity
    let whiteCase = makeVec4 (rgb, alpha)
    let gl_FragColor = ifThenElse isWhite whiteCase (Constant (0, 0, 0, 0))
    let program = makeBasicProgram gl_Position gl_FragColor

    emitProgram program
    
