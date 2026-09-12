- Scopalatro: Une carte posée sur le tapis conserve sa capacité et est aussi activée quand cette carte est capturée au lieu de seulement capturante (quand cette carte capture d'autres cartes) Ainsi cette mécanique peut se retourner contre le joueur s'il ne fait pas attention

- Scopalatro: Transformer la probabilité de jouage d'une attaque ennemie en la probabilité d'apparition de cette attaque en tant qu'effet lors de la création du deck (ie: si venom spit a une chance de 1/6 d'apparaitre alors 16% du deck sera grossièrement constitué de venom spit). Le but de ce changement est de transformer l'ennemi qui attaque et défend sans rien faire d'autre en ennemi qui joue aussi à la scopa

- TetoEngine: Créer un système de rendering plus opti et moins général, plutot créer des outils pour créer des renderers au lieu de fournir un unique InstanceRenderer.
	- Capacité de gérer des pools de mémoire partagéés entre les différents renderers
	- Gérer multiples textures (garder le système d'indirection en place ? TextureOffset qui pointe vers un élément d'un Atlas de Texture ? mais dans ce cas le centraliser plus dans la classe TextureAtlas)
	- Retirer la capacité d'avoir un nombre infini de textures ? en tout cas avoir l'utilisateur créer ça lui même s'il en a envie

- TetoEngine: On peut modifier un shader puis ensuite le compiler on the fly pour hardcoder des trucs qui changeront jamais (ou quasiment jamais) dans la durée de vie du programe (par exemple si pour une carte ou une healthbar je veux une texture en particulier je peux juste le hardcoder avant le main ou je met juste sa position dans le TextureAtlas)