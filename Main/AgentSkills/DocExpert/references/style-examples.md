# Documentation Style Examples

These examples represent the project's "Gold Standard" for Doxygen comments, including LaTeX math and advanced tagging.

## Class Summary with LaTeX Math

Use `@f$` for inline math and `@f[` / `@f]` for block math.

```cpp
/*! @brief Represents a line segment in space.

Points @f$ \overrightarrow{P} @f$ on a line are modeled using the equation

@f[
	\overrightarrow{P} = \overrightarrow{O} + t\overrightarrow{D}
@f]

where @f$ \overrightarrow{O} @f$ is the line origin and 
@f$ \overrightarrow{D} @f$ is the line direction.
*/
template<typename T>
class TLineSegment { ... };
```

## Method with Parameters, Return, and @p Tag

Use `@p` to reference parameter names within the description text.

```cpp
/*! @brief A line extending form @p origin to the furthest point possible along @p direction.
*/
TLineSegment(const TVector3<T>& origin, const TVector3<T>& direction);

/*! @brief Get the coordinates on minimum parametric distance.
@return The coordinates at minT.
*/
TVector3<T> getTail() const;
```

## Grouping and Member References

Use `///@{` and `///@}` to group related members, and `@ref` to link to other methods.

```cpp
/*! @name Getters
Basic getters for line attributes. See corresponding setters (if present)
for more info.
*/
///@{
const TVector3<T>& getOrigin() const;
const TVector3<T>& getDir() const;
///@}

/*! @brief Set the parametric range where the segment extends.
The range is [minT, maxT). This is equivalent to calling @ref setMinT(T) 
and @ref setMaxT(T) together.
*/
void setRange(T minT, T maxT);
```

## Simple Method Summary

```cpp
/*! @brief Perform rendering.
*/
virtual void doRender() = 0;
```
