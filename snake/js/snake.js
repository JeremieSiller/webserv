let score = 0;

let direction = "WEST";

const randomNumber = (max, min) =>
{
	return Math.floor((Math.random() * max) + min);
}

let game = [{x: 0, y: 0}];

const randomSnack = () =>
{
	let snack = document.getElementById("snack");
	let num = game[randomNumber(game.length - 1, 0)];
	snack.style.left = num.x + "px";
	snack.style.top = num.y + "px";
}

let snake = [ {x: 0,y: 0}]

const getscore= () =>
{
	return score;
}

const endgame = () =>
{
	document.getElementById("startbtn").style.display = "initial";
	while (snake.length > 2)
		snake.pop();
	while (game.length > 1)
		game.pop();
	game[0] = {x:0, y:0};
	score = 0;

}

let is_alive =true;

document.addEventListener("keydown", event =>
{
	if (event.key == "w" && direction != "SOUTH")
		direction = "NORTH";
	else if (event.key == "a" && direction != "WEST")
		direction = "EAST";
	else if (event.key == "s" && direction != "NORTH")
		direction = "SOUTH"
	else if (event.key == "d" && direction != "EAST")
		direction = "WEST"
});

const checkSchwanz = () =>
{

	for (let i = 1; i < snake.length; i++)
	{
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
		{
			is_alive = false;
			break ;
		}
	}
}

const moveSnake = () =>
{
	let i;
	for ( i = snake.length - 1; i > 0; i--)
	{
		snake[i].x = snake[i - 1].x;
		snake[i].y = snake[i - 1].y;
	}
	

	if (direction == "WEST")
		snake[0].x += 20;
	if (direction == "EAST")
		snake[0].x -= 20;
	if (direction == "NORTH")
		snake[0].y -= 20;
	if (direction == "SOUTH")
		snake[0].y += 20;
	if (snake[0].x < 0 || snake[0].x + 20 > 1280 || snake[0].y < 0 || snake[0].y + 20 > 720)
		is_alive = false;
	checkSchwanz();
}

const drawSnake = () =>
{
	let node = document.getElementsByClassName("c-gameSnakeBody");
	while (node[0])
		node[0].parentNode.removeChild(node[0]);

	for (let i = 0; i < snake.length; i++)
	{
		let newSnake = document.createElement("span");
		newSnake.setAttribute('class', "c-gameSnakeBody");
		newSnake.style.left = snake[i].x + "px";
		newSnake.style.top = snake[i].y + "px";
		newSnake.style.zIndex = 2;
		document.getElementById("gamesection").appendChild(newSnake);
	}
}


const gameloop = () =>
{
	if (is_alive)
	{
		setTimeout(function onTick()
		{
			drawSnake();
			let snack = document.getElementById("snack");
			if (parseInt(snack.style.left) == snake[0].x && parseInt(snack.style.top) == snake[0].y )
			{

				randomSnack();
				snake.push({x: snake[snake.length - 1].x, y: snake[snake.length - 1].y})
				score++;
				let scoreboard = document.getElementById("scoreboard");
				scoreboard.innerHTML = "score: " + score;
			}
			moveSnake();
			gameloop();
		}, 100)
	}
	else
	{
		endgame();
	}
}

const start = ()=>
{
	for (let i = 0; i < 720; i += 20)
	{
		for (let j = 0;j < 1280; j += 20)
		{
			game.push({x:j, y:i});
		}
	}
	snake[0].x = 640;
	snake[0].y = 360;	
	snake.push({x:snake[0].x - 25, y:snake[0].y});
	snake.push({x:snake[0].x - 50, y:snake[0].y});
	let scoreboard = document.getElementById("scoreboard");
	scoreboard.innerHTML = "score: 0";
	document.getElementById("startbtn").style.display = "none";
	is_alive = true;
	randomSnack();
	gameloop();
}
