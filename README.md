# Gentzen: A Logic Engine

```code
     ___
    /\  \
   _\ \  \
  /\ \ \  \
 _\ \ \ \  \_____________
/\ \ \ \ \_______________\
\ \ \ \/ /  _____________/
 \ \ \  /  /    /\_____\
  \ \/ /  /_____\/__   /
   \  /  /________\/  /
    \/_______________/

Gentzen: A Logic Engine v0.1
```

The Gentzen Engine logo. Note that the "E" is backwards on purpose; it denotes the existential quantifier "$\exists$".

An expression $\varphi$ is invalid if $\exists x_i \ (\varphi \land \lnot \varphi)$, where $x_i \in S$ and $S$ is the set of all "variable" propositions in $\varphi$. In other words, $\varphi$ is invalid if there exists an interpretation of it where $\varphi$ is simultaneously true and false. If $\varphi$ is valid, it has one of the following valuations:

$$
    \begin{align*}
        \operatorname{Always} \varphi & := \forall x_i \ \varphi \\
        \operatorname{Unknown} \varphi & := \exists x_i \ \varphi \land \exists x_i \ \lnot \varphi \\
        \operatorname{Never} \varphi & := \forall x_i \ \lnot \varphi
    \end{align*}
$$
